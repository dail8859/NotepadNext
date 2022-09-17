// docks/FunctionListDock.cpp
// started 20220903 afo@wega131

#include "FunctionListDock.h"
#include "ui_FunctionListDock.h"

#include <QTreeWidgetItem>
#include <QFile>
#include <QMessageBox>

#include<vector>

#ifdef _MSC_VER
#define popen(s,m)		_popen(s,m)
#define pclose(s)		_pclose(s)
#endif

struct FunctionListTagItem;
typedef std::vector<FunctionListTagItem> FunctionListTagList;

struct FunctionListTagItem
{
	FunctionListTagItem(const QString& name, const QString& line, const QString& type, const QString& scope)
		: m_name(name)
		, m_line(line)
		, m_type(type)
		, m_scope(scope)
	{}

	static bool hasScope(const QString& name)
	{
		for(auto prefix : { "class:", "enum:", "function:", "struct:", "namespace:", })
		{
			if(name.startsWith(prefix)) { return true; }
		}
		return false;
	}
	bool hasScope() const { return hasScope(m_scope); }

	bool isMacro() const { return m_type == "macro"; }
	bool isScope() const { return m_scope == "class" || m_scope == "enum" || m_scope == "struct" || m_scope == "namespace"; }

	QString m_name;
	QString m_line;
	QString m_type;
	QString m_info;
	QString m_scope;
};

static FunctionListTagList createTagItemList(const QString& path,const QString& ctagsCmd)
{
	QString tag_cmd(ctagsCmd);
	tag_cmd += " --sort=yes --fields=nKs -f - ";
	tag_cmd += path;

	FunctionListTagList tagList;
	FILE* fp = ::popen(tag_cmd.toStdString().c_str(), "r");
	if(nullptr != fp)
	{
		char buf[1024];
		for(int ndx=0 ; nullptr != ::fgets(buf,sizeof(buf),fp); ++ndx)
		{
			// skip empty lines and comments
			if('\0' == buf[0] || '\n' == buf[0] || '\r' == buf[0] || '!' == buf[0]) { continue; }

			QString data(QString(buf).trimmed());
			QStringList sv(data.split("\t"));
			if(sv.size() < 5) { continue ; }

			int lastIndex = sv.size()-1;
			// scope: last field starts with 'class:ABC' or 'namespace:ABC' or 'struct:ABC' or 'enum:ABC' or 'function:ABC'
			QString scope;	// c++11 lambdas : 'function:MyClass::ABC'
			if(FunctionListTagItem::hasScope(sv[ lastIndex ])) { scope = sv[lastIndex]; lastIndex--; }

			FunctionListTagItem tagItem(sv[0], sv[ lastIndex ].remove(0,5), sv[ lastIndex-1 ], scope);  // name,line,type, scope
			int pos1 = data.indexOf("/^");			// get function info
			int pos2 = data.indexOf("/;\"", pos1);	// e.g. 'xxx /^  MyFunc(args)/;"' => 'MyFunc(args)'
			if(-1 != pos1 && -1 != pos2) { tagItem.m_info = data.mid(pos1, pos2-pos1); }
			else { tagItem.m_info = sv[ lastIndex-2 ]; }

			tagList.push_back(tagItem);
		}
		::pclose(fp);
	}
	else { ::perror("popen ctags failed"); }

	return tagList;
}


FunctionListDock::FunctionListDock(QWidget *parent)
	: QDockWidget(parent)
    , ui(new Ui::FunctionListDock)
{
    ui->setupUi(this);

	QPalette pal(ui->treeWidget->palette());
	pal.setColor(QPalette::Base, QColor(220, 220, 220));
	ui->treeWidget->setPalette(pal);
	ui->treeWidget->setColumnCount(2);
	ui->treeWidget->setIndentation(10);

	connect(ui->treeWidget, &QTreeWidget::itemClicked, this, [=](QTreeWidgetItem* item, int /* column */) {
		QString strLine(item->text(0));
		if(0 == item->childCount() && strLine != "") { emit itemActivated(strLine.toInt()); }
    });
}

FunctionListDock::~FunctionListDock()
{
    delete ui;
}

static QTreeWidgetItem* findScopeItem(const QString& scope, QTreeWidget* treeW, std::vector<QTreeWidgetItem*>& scopeVec)
{
	for(auto itemW : scopeVec) { if(itemW->text(0) == scope) { return itemW; } }

	auto itemW = new QTreeWidgetItem(treeW, { scope });
	itemW->setFirstColumnSpanned(true);
	if('c' == scope[0]) { itemW->setForeground(0, QColor(160, 0, 0)); }			// class:
	else if('n' == scope[0]) { itemW->setForeground(0, QColor(34, 140, 34)); }	// namespace:
	else if('s' == scope[0]) { itemW->setForeground(0, QColor(70, 70, 230)); }	// struct:
	else if('e' == scope[0]) { itemW->setForeground(0, QColor(0, 0, 120)); }	// enum:

	scopeVec.push_back(itemW);
	return itemW;
}

void FunctionListDock::update(const QString& path)
{
	m_currentPath = path;
	if(path.isEmpty()) { return ; }	// new file

	ui->treeWidget->clear();

	if(m_ctagsCmd.isEmpty() || false == QFile::exists(m_ctagsCmd))
	{
		static bool showWarning = true;
		if(showWarning)
		{
			QMessageBox::warning(this, tr("Error FunctionList update"),  tr("Missing ctags command\n\n%1\n\nsetup via Settings/Preferences").arg(m_ctagsCmd));
			showWarning = false;	// just once!
		}
		return;
	}

	auto globalItemW = new QTreeWidgetItem({ "[ globals ]" });
	globalItemW->setForeground(0, QColor(222, 140, 0));

	auto macroItemW = new QTreeWidgetItem({ "[ macros ] " });
	macroItemW->setForeground(0, QColor(0, 0, 160));

	auto tagList(createTagItemList(m_currentPath, m_ctagsCmd));

	std::vector<QTreeWidgetItem*> scopeVec;
	for(auto const& item : tagList)
	{
		if(item.isScope()) { continue ; } // skip (class, enum, struct, namespace) created as parents by members

		QTreeWidgetItem* parentItemW = nullptr;
		if(item.hasScope()) { parentItemW = findScopeItem(item.m_scope, ui->treeWidget, scopeVec); }
		else if(item.isMacro()) { parentItemW = macroItemW; }
		else { parentItemW = globalItemW; }

		QTreeWidgetItem* itemW = new QTreeWidgetItem(parentItemW, { item.m_line, item.m_name } );
		itemW->setToolTip(0, item.m_info);
		itemW->setToolTip(1, item.m_info);
	}
	if(globalItemW->childCount() > 0)	{ ui->treeWidget->addTopLevelItem(globalItemW); }
	if(macroItemW->childCount() > 0)	{ ui->treeWidget->addTopLevelItem(macroItemW); }

	globalItemW->setFirstColumnSpanned(true);
	macroItemW->setFirstColumnSpanned(true);

	QTreeWidgetItem* scopeItemW = globalItemW;
	for(auto itemW : scopeVec)
	{
		if(itemW->childCount() > scopeItemW->childCount()) { scopeItemW = itemW; }
	}
	scopeItemW->setExpanded(true);	// expand item with greatest number of children

	ui->treeWidget->sortItems(1,Qt::AscendingOrder);
}

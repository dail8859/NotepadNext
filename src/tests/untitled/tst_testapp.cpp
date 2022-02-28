#include <QtTest>
#include <QCoreApplication>

// add necessary includes here

class TestApp : public QObject
{
    Q_OBJECT

public:
    TestApp();
    ~TestApp();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_case1();

};

TestApp::TestApp()
{

}

TestApp::~TestApp()
{

}

void TestApp::initTestCase()
{

}

void TestApp::cleanupTestCase()
{

}

void TestApp::test_case1()
{

}

QTEST_MAIN(TestApp)

#include "tst_testapp.moc"

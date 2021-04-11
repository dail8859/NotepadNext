local L = {}

L.lexer = "baanc"

L.extensions = {
	"bc",
	"cln",
}

L.keywords = {
	[0] = "alike all and array as asc at avg base based between boolean both break bset buffer by call case cast chart clear clearunref common const continue count cross current_date current_timestamp date date.num date.to.num debug default delete deleteempty deleteerror desc dim distinct dll dllusage domain double else empty end endcase enddelete enddllusage endfor endfunctionusage endif endselect endupdate endwhile enum_description eq escape exists extern false fatal fetching field first fixed for from full function functionusage ge global goto group gt having hint hints if in include index inner input inrange integer is join label last le leading left like long lt max mb menu message min multibyte ne no nodebug not notransactions nowarnings null on or order ordered outer path pi prepared print prompt question raw real ref reference refers repeat retry return right row rows select selectbind selectdo selectempty selecteos selecterror session set setunref size skip static step sticky stop strict_boolean string subhint sum table text_content then timestamp to trailing trim true union unref until update updateempty updateerror use used void warning warnings when where wherebind whereused while with",
	[1] = "abort abort.io db.after.delete db.after.insert db.after.update xmlSetData zoom.to$",
	[2] = "~.is.mandatory",
	[3] = "after.choice: after.delete: after.display: after.field: after.form: after.group: after.input: after.layout: after.read: after.rewrite: after.skip.delete: after.skip.rewrite: after.skip.write: after.write: after.zoom: before.checks:  before.choice: before.delete: before.display: before.field: before.form: before.group: before.input: before.layout: before.print: before.read: before.rewrite: before.write: before.zoom: check.input: domain.error: init.field: init.form: init.group: on.choice: on.entry: on.exit: on.input: read.view: ref.display: ref.input: selection.filter: when.field.changes:",
	[4] = "after.~: before.~: choice.~: choice.abort.program: choice.abort.work: choice.add.set: choice.ask.helpinfo: choice.attachments: choice.bms: choice.calculator: choice.calender: choice.cancel.work: choice.change.frm: choice.change.order: choice.cmd.about.baan: choice.cmd.copy: choice.cmd.cut: choice.cmd.options: choice.cmd.paste: choice.cont.process: choice.create.job: choice.current.work: choice.cust.grid: choice.def.find: choice.display.set: choice.dupl.occur: choice.end.program: choice.field.browse: choice.field.help: choice.find.data: choice.first.frm: choice.first.set: choice.first.view: choice.freeze.columns: choice.get.defaults: choice.global.copy: choice.global.delete: choice.glossary: choice.help.index: choice.hide.fields: choice.interrupt: choice.last.frm: choice.last.set: choice.last.view: choice.mark.delete: choice.mark.occur: choice.modify.set: choice.next.frm: choice.next.set: choice.next.view: choice.next.work: choice.prev.frm: choice.prev.set: choice.prev.view: choice.print.data: choice.processes: choice.recover.set: choice.reference: choice.resize.frm: choice.rotate.curr: choice.run.job: choice.save.defaults: choice.select.all: choice.send.data: choice.sendto.desktop: choice.sendto.editor: choice.sendto.mail: choice.sendto.tasklst: choice.start.chart: choice.start.query: choice.start.set: choice.suspend.work: choice.switch.windows: choice.synchronize: choice.text.manager: choice.to.clipboard: choice.unfreeze.colum: choice.update.db: choice.user.0: choice.user.1: choice.user.2: choice.user.3: choice.user.4: choice.user.5: choice.user.6: choice.user.7: choice.user.8: choice.user.9: choice.using.help: choice.zoom:  declaration: detail.~: field.~: footer.~: form.~: functions: group.~: header.~:  main.table.io: on.display.total.line: on.error: zoom.from.~:",
	[5] = "actual.occ zoomreturn$",
	[6] = "__file__ z.session",
	[7] = "^tcyesno. ^tdsls.corg.",
	[8] = "dalnew dalupdate",
}

L.styles = {
	["DEFAULT"] = {
		id = 0,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENT"] = {
		id = 1,
		fgColor = rgb(0x808040),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 3,
	},
	["COMMENT DOC"] = {
		id = 2,
		fgColor = rgb(0x004000),
		bgColor = rgb(0xFCFDDB),
		fontStyle = 3,
	},
	["NUMBER"] = {
		id = 3,
		fgColor = rgb(0xFF0000),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["KEYWORDS"] = {
		id = 4,
		fgColor = rgb(0x008080),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["STRING"] = {
		id = 5,
		fgColor = rgb(0xFF0080),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["PREPROCESSOR"] = {
		id = 6,
		fgColor = rgb(0x0000A0),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["OPERATOR"] = {
		id = 7,
		fgColor = rgb(0x800040),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["IDENTIFIER"] = {
		id = 8,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["STRING EOL NC"] = {
		id = 9,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xE0C0E0),
		fontStyle = 3,
	},
	["FUNCTIONS"] = {
		id = 10,
		fgColor = rgb(0x0000FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["FUNCTIONS ABRIDGED"] = {
		id = 11,
		fgColor = rgb(0x0000FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["SUB SECTIONS"] = {
		id = 12,
		fgColor = rgb(0xFF4500),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["MAIN SECTIONS"] = {
		id = 13,
		fgColor = rgb(0x800080),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["PREDEFINED VARIABLE"] = {
		id = 14,
		fgColor = rgb(0x800040),
		bgColor = rgb(0xA5EFF5),
		fontStyle = 1,
	},
	["PREDEFINED ATTRIBUTES"] = {
		id = 15,
		fgColor = rgb(0xFF0080),
		bgColor = rgb(0x98F3A4),
		fontStyle = 1,
	},
	["ENUM DOMAINS"] = {
		id = 16,
		fgColor = rgb(0x000080),
		bgColor = rgb(0xFFFF80),
		fontStyle = 1,
	},
	["USER DEFINED"] = {
		id = 17,
		fgColor = rgb(0x800040),
		bgColor = rgb(0x8AFFC5),
		fontStyle = 1,
	},
	["TABLE DEFINITIONS"] = {
		id = 18,
		fgColor = rgb(0x9932CC),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 5,
	},
	["TABLE SQL"] = {
		id = 19,
		fgColor = rgb(0x8A2BE2),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["DLL FUNCTIONS"] = {
		id = 20,
		fgColor = rgb(0x000080),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["DOMAIN DEFINITIONS"] = {
		id = 21,
		fgColor = rgb(0xB8860B),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 5,
	},
	["FUNCTION DEFINITIONS"] = {
		id = 22,
		fgColor = rgb(0xB22222),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 5,
	},
	["OBJECT DEFINITIONS"] = {
		id = 23,
		fgColor = rgb(0x8080FF),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 5,
	},
	["PREPROC DEFINITIONS"] = {
		id = 24,
		fgColor = rgb(0x556B2F),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 5,
	},
}
return L

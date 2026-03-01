local L = {}

L.lexer = "asn1"

L.extensions = {
	"mib",
}

L.keywords = {
	[0] = "ACCESS AGENT AUGMENTS BEGIN BITS CAPABILITIES CHOICE COMPLIANCE CONTACT CONVENTION DEFINITIONS DEFVAL DESCRIPTION DISPLAY END ENTERPRISE EXPORTS FALSE FROM GROUP GROUPS HINT IDENTITY IMPLIED IMPORTS INCLUDES INDEX INFO LAST MANDATORY MAX MIN MODULE NOTATION NOTIFICATION NULL OBJECTS OBJECT-TYPE OF ORGANIZATION PRODUCT RELEASE REFERENCE REQUIRES REVISION SEQUENCE SIZE STATUS SUPPORTS SYNTAX TEXTUAL TRAP TYPE TRAP-TYPE UPDATED VALUE VARIABLES VARIATION WRITE",
	[1] = "accessible create current deprecated for mandatory not notify not-accessible obsolete only optional read read-only read-write write",
	[2] = "ABSENT ANY APPLICATION BIT BOOLEAN BY COMPONENT COMPONENTS DEFAULT DEFINED ENUMERATED EXPLICIT EXTERNAL IMPLICIT INIFINITY MAX MIN MINUS OPTIONAL PRESENT PRIVATE REAL SET TAGS TRUE",
	[3] = "Counter Counter32 Counter64 DisplayString Gauge Gauge32 IDENTIFIER INTEGER Integer32 IpAddress NetworkAddress NsapAddress OBJECT OCTET Opaque PhysAddress STRING TimeTicks UInteger32 UNITS Unsigned32",
}

L.styles = {
	["DEFAULT"] = {
		id = 0,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
	},
	["COMMENT"] = {
		id = 1,
		fgColor = rgb(0x007F00),
		bgColor = rgb(0xFFFFFF),
	},
	["IDENTIFIERS"] = {
		id = 2,
		fgColor = rgb(0x000000),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["DOUBLE QUOTED STRING"] = {
		id = 3,
		fgColor = rgb(0x7F007F),
		bgColor = rgb(0xFFFFFF),
	},
	["NUMERIC OID DEFINITION"] = {
		id = 4,
		fgColor = rgb(0x007F7F),
		bgColor = rgb(0xFFFFFF),
		fontStyle = 1,
	},
	["NON OID NUMBERS"] = {
		id = 5,
		fgColor = rgb(0x7F0000),
		bgColor = rgb(0xFFFFFF),
	},
	["KEYWORDS"] = {
		id = 6,
		fgColor = rgb(0x00007F),
		bgColor = rgb(0xFFFFFF),
	},
	["ATTRIBUTES"] = {
		id = 7,
		fgColor = rgb(0xF07800),
		bgColor = rgb(0xFFFFFF),
	},
	["DESCRIPTORS"] = {
		id = 8,
		fgColor = rgb(0x00007F),
		bgColor = rgb(0xFFFFFF),
	},
	["TYPES"] = {
		id = 9,
		fgColor = rgb(0x00007F),
		bgColor = rgb(0xFFFFFF),
	},
	["OPERATORS"] = {
		id = 10,
		fgColor = rgb(0x222222),
		bgColor = rgb(0xFFFFFF),
	},
}
return L

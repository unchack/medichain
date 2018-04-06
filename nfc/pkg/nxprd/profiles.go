package nxprd

type TagCoordinate struct {
	ExpectedBlock 			int
	ExpectedIndex 			int
	ExpectedByte   			byte
}
//NFC Forum Type 2 Tag and ISO/IEC14443 Type A Profiles
type TagProfile struct {
	ProfileName						string
	SupplierName					string
	IdentificationByte				TagCoordinate
	TagLengthByte					TagCoordinate
	InternalByte					TagCoordinate
	UserBlockStartIndex 			int
	UserBlockEndIndex   			int
	TotalBlocks         			int
	AuthenticationSupport			bool
	PasswordBlockIndex				int
}

var MifareULProfile = TagProfile{
	SupplierName:		 "Mifare",
	ProfileName:	     "MifareUL",
	UserBlockStartIndex:        4,
	UserBlockEndIndex:         15,
	TotalBlocks:               16,
	AuthenticationSupport:  false,
}

var NTAG203Profile = TagProfile{
	SupplierName:		    "NXP",
	ProfileName:		    "NTAG203",
	IdentificationByte:    TagCoordinate{
		ExpectedBlock:         0,
		ExpectedIndex:         0,
		ExpectedByte:          0x04,
	},
	TagLengthByte:         TagCoordinate{
		ExpectedBlock:     	   3,
		ExpectedIndex:     	   2,
		ExpectedByte:          0x12,
	},
	InternalByte:      TagCoordinate{
		ExpectedBlock:          2,
		ExpectedIndex:          1,
		ExpectedByte:           0x00,
	},
	UserBlockStartIndex:        4,
	UserBlockEndIndex:         39,
	TotalBlocks:               41,
	AuthenticationSupport:  false,
}

var NTAG213Profile = TagProfile{
	SupplierName:		    "NXP",
	ProfileName:		    "NTAG213",
	IdentificationByte:     TagCoordinate{
		ExpectedBlock:         0,
		ExpectedIndex:         0,
		ExpectedByte:          0x04,
	},
	TagLengthByte:          TagCoordinate{
		ExpectedBlock:     	   3,
		ExpectedIndex:     	   2,
		ExpectedByte:          0x12,
	},
	InternalByte:      TagCoordinate{
		ExpectedBlock:          2,
		ExpectedIndex:          1,
		ExpectedByte:           0x48,
	},
	UserBlockStartIndex:        4,
	UserBlockEndIndex:         39,
	TotalBlocks:               44,
	AuthenticationSupport:   true,
	PasswordBlockIndex:		   43,
}

var NTAG215Profile = TagProfile{
	SupplierName:		      "NXP",
	ProfileName:		      "NTAG215",
	IdentificationByte:       TagCoordinate{
		ExpectedBlock:           0,
		ExpectedIndex:           0,
		ExpectedByte:            0x04,
	},
	TagLengthByte:            TagCoordinate{
		ExpectedBlock:       	 3,
		ExpectedIndex:       	 2,
		ExpectedByte:            0x3E,
	},
	UserBlockStartIndex:          4,
	UserBlockEndIndex:          129,
	TotalBlocks:                134,
	AuthenticationSupport:	   true,
	PasswordBlockIndex:		    133,
}

var NTAG216Profile = TagProfile{
	SupplierName:		     "NXP",
	ProfileName:		     "NTAG216",
	IdentificationByte:      TagCoordinate{
		ExpectedBlock:          0,
		ExpectedIndex:          0,
		ExpectedByte:           0x04,
	},
	TagLengthByte:           TagCoordinate{
		ExpectedBlock:      	 3,
		ExpectedIndex:      	 2,
		ExpectedByte:            0x6D,
	},
	UserBlockStartIndex:         4,
	UserBlockEndIndex:         225,
	TotalBlocks:               230,
	AuthenticationSupport:	  true,
	PasswordBlockIndex:		   229,
}

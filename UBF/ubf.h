#define UBF_MAXTAGLEN 32

#define UBF_TYPES_UNKNOWN 0

#define UBF_TYPES_INT			(1 << 6)
#define UBF_TYPES_SBYTE			(UBF_TYPES_INT + 1)
#define UBF_TYPES_BYTE			(UBF_TYPES_INT + 2)
#define UBF_TYPES_SWORD			(UBF_TYPES_INT + 3)
#define UBF_TYPES_WORD			(UBF_TYPES_INT + 4)
#define UBF_TYPES_SDWORD		(UBF_TYPES_INT + 5)
#define UBF_TYPES_DWORD			(UBF_TYPES_INT + 6)
#define UBF_TYPES_SQWORD		(UBF_TYPES_INT + 7)
#define UBF_TYPES_QWORD			(UBF_TYPES_INT + 8)
#define UBF_TYPES_NUMINT		(9)

#define UBF_TYPES_FLOAT			(2 << 6)
#define UBF_TYPES_FLOAT4		(UBF_TYPES_FLOAT + 1)
#define UBF_TYPES_FLOAT8		(UBF_TYPES_FLOAT + 2)
#define UBF_TYPES_FLOAT10		(UBF_TYPES_FLOAT + 3)
#define UBF_TYPES_FLOAT4		(UBF_TYPES_FLOAT + 4)

#define UBF_TYPES_STRING8		128
#define UBF_TYPES_LENSTRING8	128
#define UBF_TYPES_STRINGZ8		129
#define UBF_TYPES_FIXEDSTRING8	130

#define UBF_TYPES_LENSTRING8	192
#define UBF_TYPES_STRINGZ8		193
#define UBF_TYPES_FIXEDSTRING8	194

class UBFManager
{




}

class UBFDictionaryEntry
{
	DWORD type;
	DWORD flags;
	char *tag;
	char *desc;
	char *help;
	void Read
};

class UBFDictionary
{


}

class UBFLoader
{




}

class UBF

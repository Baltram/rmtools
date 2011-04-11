#include "rmPropertyLinker.h"

namespace rmDummyClass
{
	rmOut DummyClassManager::out("rmDummyClasses.cpp");

	const rmLPCChar DummyClassManager::DummyClassNames[EDummyClassCount] = {
		"bCAccessorProperty",
		"bCAccessorPropertyObject",
		"bCEulerAngles",
		"bCFloatColor",
		"bCGuid",
		"bCImageOrMaterialResourceString",
		"bCImageResourceString",
		"bCRect",
		"bCString",
		"bCVector2",
		"bool",
		"char",
		"eCColorScale",
		"eCColorSrcBase",
		"eCColorSrcBlend",
		"eCColorSrcCamDistance",
		"eCColorSrcCamObstacle",
		"eCColorSrcCombiner",
		"eCColorSrcConstant",
		"eCColorSrcConstantAnimation",
		"eCColorSrcConstantSwitch",
		"eCColorSrcCubeSampler",
		"eCColorSrcFresnel",
		"eCColorSrcMask",
		"eCColorSrcProxy",
		"eCColorSrcSampler",
		"eCColorSrcSkydomeSampler",
		"eCColorSrcVertexColor",
		"eCMaterialResource2",
		"eCShaderBase",
		"eCShaderDefault",
		"eCShaderEllementBase",
		"eCShaderLeafs",
		"eCShaderLightStreaks",
		"eCShaderParticle",
		"eCShaderSkyDome",
		"eCTexCoordSrcBase",
		"eCTexCoordSrcBumpOffset",
		"eCTexCoordSrcColor",
		"eCTexCoordSrcOscillator",
		"eCTexCoordSrcProxy",
		"eCTexCoordSrcReflect",
		"eCTexCoordSrcRotator",
		"eCTexCoordSrcScale",
		"eCTexCoordSrcScroller",
		"float",
		"int"
	};

	const rmLPCChar DummyClassManager::DummyTemplateClassNames[EDummyTemplateClassCount] = {
		"bTPropertyContainer"
	};

	const rmLPCChar DummyClassManager::enumNames[NEnums] = {
		"eEColorSrcCombinerType",
		"eEColorSrcSampleTexRepeat",
		"eEColorSrcSwitchRepeat",
		"eEDistanceType",
		"eEFresnelTerm",
		"eEReflectType",
                "eEShaderColorModifier", //$!
                "eEShaderColorSrcComponent", //$!"
		"eEShaderMaterialAmbientType",
		"eEShaderMaterialBRDFType",
		"eEShaderMaterialBlendMode",
		"eEShaderMaterialTransformation",
		"eEShaderMaterialVersion",
		"eEShapeMaterial",
		"eETexCoordSrcOscillatorType",
		"eETexCoordSrcRotatorType"
	};

	const rmLPCChar DummyClassManager::enumValueStrings[][MaxEnumValueCount] = {
                {
			"eEColorSrcCombinerType_Add",
			"eEColorSrcCombinerType_Subtract",
			"eEColorSrcCombinerType_Multiply",
			"eEColorSrcCombinerType_Max",
			"eEColorSrcCombinerType_Min"
		},
		{
			"eEColorSrcSampleTexRepeat_Wrap",
			"eEColorSrcSampleTexRepeat_Clamp",
			"eEColorSrcSampleTexRepeat_Mirror"
		},
		{
			"eEColorSrcSwitchRepeat_Repeat",
			"eEColorSrcSwitchRepeat_Clamp",
			"eEColorSrcSwitchRepeat_PingPong"
		},
		{
			"eEDistanceType_Src",
			"eEDistanceType_Dest",
			"eEDistanceType_Delta"
		},
		{
			"eEFresnelTerm_Simple",
			"eEFresnelTerm_Quadric",
			"eEFresnelTerm_Power"
		},
		{
			"eEReflectType_Reflect",
			"eEReflectType_WorldEye",
			"eEReflectType_WorldNormal",
			"eEReflectType_TangentNormal",
			"eEReflectType_TangentEye"
		},
                {
                        "eEShaderColorModifier_Default",
                        "eEShaderColorModifier_Negate",
                        "eEShaderColorModifier_Invert",
                        "eEShaderColorModifier_Saturate",
                        "eEShaderColorModifier_Ceil",
                        "eEShaderColorModifier_Floor",
                        "eEShaderColorModifier_Abs",
                        "eEShaderColorModifier_Frac"

                },
                {
                        "eEShaderColorSrcComponent_Default",
                        "eEShaderColorSrcComponent_RGB",
                        "eEShaderColorSrcComponent_R",
                        "eEShaderColorSrcComponent_G",
                        "eEShaderColorSrcComponent_A"

                },
		{
			"eEShaderMaterialAmbientType_None",
			"eEShaderMaterialAmbientType_Simple",
			"eEShaderMaterialAmbientType_Complex"
		},
		{
			"eEShaderMaterialBRDFType_Simple",
			"eEShaderMaterialBRDFType_Complex",
			"eEShaderMaterialBRDFType_WrapAround"
		},
		{
			"eEShaderMaterialBlendMode_Normal",
			"eEShaderMaterialBlendMode_Masked",
			"eEShaderMaterialBlendMode_AlphaBlend",
			"eEShaderMaterialBlendMode_Modulate",
			"eEShaderMaterialBlendMode_AlphaModulate",
			"eEShaderMaterialBlendMode_Translucent",
			"eEShaderMaterialBlendMode_Darken",
			"eEShaderMaterialBlendMode_Brighten",
			"eEShaderMaterialBlendMode_Invisible"
		},
		{
			"eEShaderMaterialTransformation_Default",
			"eEShaderMaterialTransformation_Instanced",
			"eEShaderMaterialTransformation_Skinned",
			"eEShaderMaterialTransformation_Tree_Branches",
			"eEShaderMaterialTransformation_Tree_Fronds",
			"eEShaderMaterialTransformation_Tree_LeafCards",
			"eEShaderMaterialTransformation_Tree_LeafMeshes",
			"eEShaderMaterialTransformation_Billboard",
			"eEShaderMaterialTransformation_Morphed",
			"eEShaderMaterialTransformation_Skinned_Morphed",
			"eEShaderMaterialTransformation_Vegetation",
			"eEShaderMaterialTransformation_Tree_Billboards"
		},
		{
			"eEShaderMaterialVersion_1_1",
			"eEShaderMaterialVersion_1_4",
			"eEShaderMaterialVersion_2_0",
			"eEShaderMaterialVersion_3_0"
		},
		{
			"eEShapeMaterial_None",
			"eEShapeMaterial_Wood",
			"eEShapeMaterial_Metal",
			"eEShapeMaterial_Water",
			"eEShapeMaterial_Stone",
			"eEShapeMaterial_Earth",
			"eEShapeMaterial_Ice",
			"eEShapeMaterial_Leather",
			"eEShapeMaterial_Clay",
			"eEShapeMaterial_Glass",
			"eEShapeMaterial_Flesh",
			"eEShapeMaterial_Snow",
			"eEShapeMaterial_Debris",
			"eEShapeMaterial_Foliage",
			"eEShapeMaterial_Magic",
			"eEShapeMaterial_Grass",
			"eEShapeMaterial_SpringAndDamper1",
			"eEShapeMaterial_SpringAndDamper2",
			"eEShapeMaterial_SpringAndDamper3"
		},
		{
			"eETexCoordSrcOscillatorType_Pan",
			"eETexCoordSrcOscillatorType_Stretch",
			"eETexCoordSrcOscillatorType_StretchRepeat",
			"eETexCoordSrcOscillatorType_Jitter"
		},
		{
			"eETexCoordSrcRotatorType_Once",
			"eETexCoordSrcRotatorType_Constant",
			"eETexCoordSrcRotatorType_Oscillate"
		}
	};

        const rmU32 DummyClassManager::enumStringListSizes[NEnums] = {5, 3, 3, 3, 3, 5, 8, 6, 3, 3, 9, 12, 4, 19, 4, 3};

	bool DummyClassManager::assignDummyClass(WrittenClassBase *& target, string name, rmU32 size)
	{
		if (name[name.size() - 1] == '>') {
			// Hopefully there are only templates taking one template argument.
			rmU32 originalClassNameSize = name.find_first_of('<');
			string templateArgument = name.substr((originalClassNameSize + 1), (name.size() - originalClassNameSize - 2));
			name.resize(originalClassNameSize);
			rmU32 result = findCStringInArray(name.c_str(), DummyTemplateClassNames, EDummyTemplateClassCount);
			switch (result)
			{
			case EbTPropertyContainer:
				{
					if (templateArgument.substr(0, min<rmU32>(templateArgument.size(), 5)) == "enum ") {
						templateArgument.replace(0, 5, "");
					}
					target = new bTPropertyContainer(templateArgument);
					break;
				}
			default:
				{
					result = (-1);
				}
			}
			if (result != (-1)) {
				return true;
			}
		}
		else {
			rmU32 result = findCStringInArray(name.c_str(), DummyClassNames, EDummyClassCount);
			switch (result)
			{
			case EGEBool:
				{
					target = new GEBool();
					break;
				}
			case EbCAccessorProperty:
				{
					target = new bCAccessorProperty();
					break;
				}
			case EbCAccessorPropertyObject:
				{
					target = new bCAccessorPropertyObject();
					break;
				}
			case EbCEulerAngles:
				{
					target = new bCEulerAngles();
					break;
				}
			case EbCFloatColor:
				{
					target = new bCFloatColor();
					break;
				}
			case EbCGuid:
				{
					target = new bCGuid();
					break;
				}
			case EbCImageOrMaterialResourceString:
				{
					target = new bCString(string(DummyClassNames[EbCImageOrMaterialResourceString]));
					break;
				}
			case EbCImageResourceString:
				{
					target = new bCString(string(DummyClassNames[EbCImageResourceString]));
					break;
				}
			case EbCRect:
				{
					target = new bCRect();
					break;
				}
			case EbCString:
				{
					target = new bCString();
					break;
				}
			case EbCVector2:
				{
					target = new bCVector2();
					break;
				}
			case EGEChar:
				{
					target = new GEChar();
					break;
				}
			case EeCColorScale:
				{
					target = new eCColorScale();
					break;
				}
			case EeCColorSrcBase:
				{
					target = new eCColorSrcBase();
					break;
				}
			case EeCColorSrcBlend:
				{
					target = new eCColorSrcBlend();
					break;
				}
			case EeCColorSrcCamDistance:
				{
					target = new eCColorSrcCamDistance();
					break;
				}
			case EeCColorSrcCamObstacle:
				{
					target = new eCColorSrcCamObstacle();
					break;
				}
			case EeCColorSrcCombiner:
				{
					target = new eCColorSrcCombiner();
					break;
				}
			case EeCColorSrcConstant:
				{
					target = new eCColorSrcConstant();
					break;
				}
			case EeCColorSrcConstantAnimation:
				{
					target = new eCColorSrcConstantAnimation();
					break;
				}
			case EeCColorSrcConstantSwitch:
				{
					target = new eCColorSrcConstantSwitch();
					break;
				}
			case EeCColorSrcCubeSampler:
				{
					target = new eCColorSrcCubeSampler();
					break;
				}
			case EeCColorSrcFresnel:
				{
					target = new eCColorSrcFresnel();
					break;
				}
			case EeCColorSrcMask:
				{
					target = new eCColorSrcMask();
					break;
				}
			case EeCColorSrcProxy:
				{
					target = new eCColorSrcProxy();
					break;
				}
			case EeCColorSrcSampler:
				{
					target = new eCColorSrcSampler();
					break;
				}
			case EeCColorSrcSkydomeSampler:
				{
					target = new eCColorSrcSkydomeSampler();
					break;
				}
			case EeCColorSrcVertexColor:
				{
					target = new eCColorSrcVertexColor();
					break;
				}
			case EeCMaterialResource2:
				{
					target = new eCMaterialResource2();
					break;
				}
			case EeCShaderBase:
				{
					target = new eCShaderBase();
					break;
				}
			case EeCShaderDefault:
				{
					target = new eCShaderDefault();
					break;
				}
			case EeCShaderEllementBase:
				{
					target = new eCShaderEllementBase();
					break;
				}
			case EeCShaderLeafs:
				{
					target = new eCShaderLeafs();
					break;
				}
			case EeCShaderLightStreaks:
				{
					target = new eCShaderLightStreaks();
					break;
				}
			case EeCShaderParticle:
				{
					target = new eCShaderParticle();
					break;
				}
			case EeCShaderSkyDome:
				{
					target = new eCShaderSkyDome();
					break;
				}
			case EeCTexCoordSrcBase:
				{
					target = new eCTexCoordSrcBase();
					break;
				}
			case EeCTexCoordSrcBumpOffset:
				{
					target = new eCTexCoordSrcBumpOffset();
					break;
				}
			case EeCTexCoordSrcColor:
				{
					target = new eCTexCoordSrcColor();
					break;
				}
			case EeCTexCoordSrcOscillator:
				{
					target = new eCTexCoordSrcOscillator();
					break;
				}
			case EeCTexCoordSrcProxy:
				{
					target = new eCTexCoordSrcProxy();
					break;
				}
			case EeCTexCoordSrcReflect:
				{
					target = new eCTexCoordSrcReflect();
					break;
				}
			case EeCTexCoordSrcRotator:
				{
					target = new eCTexCoordSrcRotator();
					break;
				}
			case EeCTexCoordSrcScale:
				{
					target = new eCTexCoordSrcScale();
					break;
				}
			case EeCTexCoordSrcScroller:
				{
					target = new eCTexCoordSrcScroller();
					break;
				}
			case EGEFloat:
				{
					target = new GEFloat();
					break;
				}
			case EGEInt:
				{
					target = new GEInt();
					break;
				}
			default:
				{
					result = (-1);
				}
			}
			if (result != (-1)) {
				return true;
			}
		}

		if (findCStringInArray(name.c_str(), enumNames, NEnums) != (-1)) {
			target = new EnumValue(name);
			return true;
		}

		if (size == (-1)) {
			OUT_SERR("Unknown class and size: " << name)
		}
		target = new HexData(size, name);
		return false;
	}

	void DummyClassManager::asciiWriteAsMember(rmOutFileStream & fs, string indent, const WrittenClassBase * data, rmLPCChar memberName, rmLPCChar typeName)
	{
		string tmp;
		if (typeName == "") {
			tmp = data->getClassName();
			typeName = tmp.c_str();
		}
		fs << indent << typeName << " " << memberName;
		if (data->isSimpleClass()) {
			fs << " // ";
			data->asciiWrite(fs, "");
			fs << newLine;
		}
		else {
			fs << " {" << newLine;
			data->asciiWrite(fs, string("\t").append(indent));
			fs << indent << "}" << newLine;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////
	// WrittenClassBase
	////////////////////////////////////////////////////////////////////////////////////

	rmOut WrittenClassBase::out = rmOut("rmDummyClasses.cpp");

	WrittenClassBase::WrittenClassBase(void)
	{
		this->isInitialized = false;
	}

        WrittenClassBase::WrittenClassBase(const WrittenClassBase & src) : simple(src.simple), isInitialized(src.isInitialized), className(src.className) {}

        WrittenClassBase::~WrittenClassBase(void) {}

	WrittenClassBase * WrittenClassBase::clone(void) const
	{
		return new WrittenClassBase(*this);
	}

	void WrittenClassBase::read(rmInFileStream & fs)
	{
		if (isInitialized) {
			OUT_SERR("Trying to read from file to dummy class which already has been read to.")
		}
		isInitialized = true;
	}

	void WrittenClassBase::write(rmOutFileStream & fs) const
	{
		if (!isInitialized) {
			OUT_SERR("Trying to write dummy class before having read in data")
		}
	}

	void WrittenClassBase::asciiWrite(rmOutFileStream & fs, string indent) const
	{
		if (!isInitialized) {
			OUT_SERR("Trying to write dummy class before having read in data")
		}
	}

	rmU32 WrittenClassBase::getWriteSize(void) const
	{
		if (!isInitialized) {
			OUT_SERR("Trying to get write size of dummy class before having read in data")
		}
		return 0;
	}

        bool WrittenClassBase::doSpecialPropAssign(string name)
        {
                if (!isInitialized) {
                        OUT_SERR("Trying to assign member variables as properties before having read in data")
                }
                return false;
        }

        void WrittenClassBase::assignVarsAsProps(void)
	{
	    if (!isInitialized) {
			OUT_SERR("Trying to assign member variables as properties before having read in data")
		}
	}

	string WrittenClassBase::getClassName(void) const
	{
                return className;
	}

	rmBool WrittenClassBase::isSimpleClass(void) const
	{
		return simple;
	}

	void WrittenClassBase::setClassName(string name)
	{
	    className = name;
	}

        ////////////////////////////////////////////////////////////////////////////////////
        // HexData
        ////////////////////////////////////////////////////////////////////////////////////


        HexData::HexData(rmU32 size, string name)
        {
                Data.resize(size);
                setClassName(name);
                simple = true;
        }


        HexData::HexData(const HexData & src) : WrittenClassBase(src), Data(src.Data) {}


        HexData::~HexData(void) {}


        WrittenClassBase * HexData::clone(void) const
        {
                return new HexData(*this);
        }


        void HexData::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                fs.read(&Data.front(), (Data.size() * sizeof(rmU8)));
        }


        void HexData::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                fs.write(&Data.front(), (Data.size() * sizeof(rmU8)));
        }


        void HexData::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);


                rmChar * buffer = new rmChar [Data.size() * 2 + 1];
                rmChar * inBuffer = buffer;
                buffer[Data.size() * 2] = 0;


                for (vector<rmU8>::const_iterator i = Data.begin(), ie = Data.end(); i != ie; ++i) {
                        *inBuffer = '0';
                        _itoa(*i, ((*i < 16) ? (inBuffer + 1) : inBuffer), 16);
                        inBuffer[0] = toupper(inBuffer[0]);
                        inBuffer[1] = toupper(inBuffer[1]);
                        inBuffer += 2;
                }


                fs << buffer;


                delete [] buffer;
        }


        rmU32 HexData::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return Data.size();
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // EnumValue
        ////////////////////////////////////////////////////////////////////////////////////


        EnumValue::EnumValue(const string & typeName)
        {
                setClassName(typeName);
                simple = true;
        }


        EnumValue::EnumValue(const EnumValue & src) : WrittenClassBase(src), Value(src.Value) {}


        EnumValue::~EnumValue(void) {}


        WrittenClassBase * EnumValue::clone(void) const
        {
                return new EnumValue(*this);
        }


        void EnumValue::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                fs.read(Value);
        }


        void EnumValue::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                fs.write(Value);
        }


        void EnumValue::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                rmChar defaultValueString[] = "<unknown representation>";
                rmLPCChar valueString = defaultValueString;
                rmU32 iEnum = findCStringInArray(getClassName().c_str(), DummyClassManager::enumNames, DummyClassManager::NEnums);
                if (iEnum != ( 1)) {
                        if (Value < DummyClassManager::enumStringListSizes[iEnum]) {
                                valueString = DummyClassManager::enumValueStrings[iEnum][Value];
                        }
                }
                fs << Value << " = " << valueString;
        }


        rmU32 EnumValue::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return sizeof(rmU32);
        }


        rmU32 & EnumValue::getValue(void)
        {
                return Value;
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // bCGuid
        ////////////////////////////////////////////////////////////////////////////////////


        bCGuid::bCGuid(void)
        {
                setClassName("bCGuid");

#ifdef BCGUID_SIMPLE
                simple = true;
#else
                simple = false;
#endif
        }


        bCGuid::bCGuid(const bCGuid & src) : WrittenClassBase(src), Valid(src.Valid), Alignment(src.Alignment)
        {
                memcpy(Guid.value, src.Guid.value, (16 * sizeof(rmU8)));
        }


        bCGuid::~bCGuid(void) {}


        WrittenClassBase * bCGuid::clone(void) const
        {
                return new bCGuid(*this);
        }


        void bCGuid::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                fs.read(Guid.value, 16 * sizeof(rmU8));
                fs.read(&Valid, sizeof(rmBool));
                Alignment.push_back(fs.readByte()); Alignment.push_back(fs.readByte()); Alignment.push_back(fs.readByte());
        }


        void bCGuid::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                fs.write(Guid.value, 16 * sizeof(rmU8));
                fs.write(&Valid, sizeof(rmBool));
                for (list<rmU8>::const_iterator i = Alignment.begin(), ie = Alignment.end(); i != ie; ++i) {
                    fs.write(*i);
                }
        }


        void bCGuid::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);

#ifndef BCGUID_SIMPLE
                fs << indent << "GUID Guid; // ";
#endif
                fs << Guid.toText();
#ifndef BCGUID_SIMPLE
                fs << newLine << indent << "GEBool valid; // " << Valid << newLine;
#else
                fs << ", " << Valid << ", ";
#endif

list<rmU8>::const_iterator alignmentIt = Alignment.begin();
#ifndef BCGUID_SIMPLE
                fs << indent << "GEU8 Alignment[3] " << *alignmentIt++ << ", " << *alignmentIt++ << ", " << *alignmentIt << newLine;
#else
                fs << *alignmentIt++ << ", " << *alignmentIt++ << ", " << *alignmentIt;
#endif
        }


        rmU32 bCGuid::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return ((16 * sizeof(rmU8)) + sizeof(rmBool) + (3 * sizeof(rmU8)));
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // GEBool
        ////////////////////////////////////////////////////////////////////////////////////


        GEBool::GEBool(void)
        {
                setClassName("GEBool");
                simple = true;
        }


        GEBool::GEBool(const GEBool & src) : WrittenClassBase(src), Value(src.Value) {}


        GEBool::~GEBool(void) {}


        WrittenClassBase * GEBool::clone(void) const
        {
                return new GEBool(*this);
        }


        void GEBool::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                Value = (fs.readByte() != 0);
        }


        void GEBool::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                fs.writeByte((Value) ? 1 : 0);
        }


        void GEBool::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                fs << Value;
        }


        rmU32 GEBool::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return sizeof(rmBool);
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // GEChar
        ////////////////////////////////////////////////////////////////////////////////////


        GEChar::GEChar(void)
        {
                setClassName("GEChar");
                simple = true;
        }


        GEChar::GEChar(const GEChar & src) : WrittenClassBase(src), Value(src.Value) {}


        GEChar::~GEChar(void) {}


        WrittenClassBase * GEChar::clone(void) const
        {
                return new GEChar(*this);
        }


        void GEChar::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                Value = fs.readByte();
        }


        void GEChar::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                fs.writeByte(Value);
        }


        void GEChar::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                fs << Value;
        }


        rmU32 GEChar::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return sizeof(rmChar);
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // GEInt
        ////////////////////////////////////////////////////////////////////////////////////


        GEInt::GEInt(void)
        {
                setClassName("GEInt");
                simple = true;
        }


        GEInt::GEInt(const GEInt & src) : WrittenClassBase(src), Value(src.Value) {}


        GEInt::~GEInt(void) {}


        WrittenClassBase * GEInt::clone(void) const
        {
                return new GEInt(*this);
        }


        void GEInt::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                fs.read(Value);
        }


        void GEInt::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                fs.write(Value);
        }


        void GEInt::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                fs << Value;
        }


        rmU32 GEInt::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return sizeof(rmI32);
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // GEFloat
        ////////////////////////////////////////////////////////////////////////////////////


        GEFloat::GEFloat(void)
        {
                setClassName("GEFloat");
                simple = true;
        }


        GEFloat::GEFloat(const GEFloat & src) : WrittenClassBase(src), Value(src.Value) {}


        GEFloat::~GEFloat(void) {}


        WrittenClassBase * GEFloat::clone(void) const
        {
                return new GEFloat(*this);
        }


        void GEFloat::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                fs.read(Value);
        }


        void GEFloat::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                fs.write(Value);
        }


        void GEFloat::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                fs << Value;
        }


        rmU32 GEFloat::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return sizeof(rmF32);
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // bCString
        ////////////////////////////////////////////////////////////////////////////////////


        bCString::bCString(void)
        {
                setClassName("bCString");
#ifdef BCSTRING_SIMPLE
                simple = true;
#else
                simple = false;
#endif


        }


        bCString::bCString(const bCString & src) : WrittenClassBase(src), Value(src.Value) {}


        bCString::bCString(string typeName)
        {
                setClassName(typeName);
#ifdef BCSTRING_SIMPLE
                simple = true;
#else
                simple = false;
#endif
        }


        bCString::~bCString(void) {}


        WrittenClassBase * bCString::clone(void) const
        {
                return new bCString(*this);
        }


        void bCString::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                fs.readString(Value, fs.readShort());
        }


        void bCString::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                fs.writeShort(Value.size());
                fs.writeString(Value);
        }


        void bCString::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
#ifndef BCSTRING_SIMPLE
                fs << indent << "GEU16 Length // " << Value.size() << newLine;
                fs << indent << "GEChar Text[" << Value.size() << "] // " << "\"" << Value << "\"" << newLine;
#else
                fs << Value.size() << ", " << "\"" << Value << "\"";
#endif
        }


        rmU32 bCString::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return (sizeof(rmU16) + Value.size());
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // bTPropertyContainer
        ////////////////////////////////////////////////////////////////////////////////////


        bTPropertyContainer::bTPropertyContainer(string templateArgument)
        {
                setClassName(string("bTPropertyContainer<").append(templateArgument).append(">"));
                simple = false;
                DummyClassManager::assignDummyClass(Value, templateArgument);
        }


        bTPropertyContainer::bTPropertyContainer(const bTPropertyContainer & src) : WrittenClassBase(src), Version(src.Version), Value((src.Value == 0) ? 0 : src.Value->clone()) {}


        bTPropertyContainer::~bTPropertyContainer(void)
        {
                delete Value;
        }


        WrittenClassBase * bTPropertyContainer::clone(void) const
        {
                return new bTPropertyContainer(*this);
        }


        void bTPropertyContainer::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                fs.read(Version);
                Value->read(fs);
        }


        void bTPropertyContainer::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                fs.write(Version);
                Value->write(fs);
        }


        void bTPropertyContainer::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                fs << indent << "GEU16 Version // " << Version << newLine;
                DummyClassManager::asciiWriteAsMember(fs, indent, Value, "Value");
        }


        rmU32 bTPropertyContainer::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return (sizeof(rmU16) + Value->getWriteSize());
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // bCAccessorProperty
        ////////////////////////////////////////////////////////////////////////////////////


        bCAccessorProperty::bCAccessorProperty(void)
        {
                setClassName("bCAccessorProperty");
                simple = false;
                Data = 0;
        }


        bCAccessorProperty::bCAccessorProperty(const bCAccessorProperty & src) : Name(src.Name), Type(src.Type), Version(src.Version), Data((src.Data == 0) ? 0 : src.Data->clone()) {}


        bCAccessorProperty::~bCAccessorProperty(void)
        {
                delete Data;
        }


        WrittenClassBase * bCAccessorProperty::clone(void) const
        {
                return new bCAccessorProperty(*this);
        }


        void bCAccessorProperty::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                Name.read(fs);
                Type.read(fs);
                fs.read(Version);
                DummyClassManager::assignDummyClass(Data, Type.Value, fs.readLong());
                Data->read(fs);
        }


        void bCAccessorProperty::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                Name.write(fs);
                Type.write(fs);
                fs.write(Version);
                fs.write(Data->getWriteSize());
                Data->write(fs);
        }


        void bCAccessorProperty::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                string newIndent = string("\t").append(indent);


                DummyClassManager::asciiWriteAsMember(fs, indent, &Name, "Name");
                DummyClassManager::asciiWriteAsMember(fs, indent, &Type, "Type");


                fs << indent << "GEU16 Version // " << Version << newLine;


                DummyClassManager::asciiWriteAsMember(fs, indent, Data, "Data");
        }


        rmU32 bCAccessorProperty::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return (Name.getWriteSize() + Type.getWriteSize() + sizeof(rmU16) + Data->getWriteSize() + sizeof(rmU32));
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // bCFloatColor
        ////////////////////////////////////////////////////////////////////////////////////


        bCFloatColor::bCFloatColor(void)
        {
                setClassName("bCFloatColor");
                simple = false;
        }


        bCFloatColor::bCFloatColor(const bCFloatColor & src) : __vftbl(src.__vftbl), R(src.R), G(src.G), B(src.B) {}


        bCFloatColor::~bCFloatColor(void) {}


        WrittenClassBase * bCFloatColor::clone(void) const
        {
                return new bCFloatColor(*this);
        }


        void bCFloatColor::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                fs.read(__vftbl);
                fs.read(R);
                fs.read(G);
                fs.read(B);
        }


        void bCFloatColor::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                fs.write(__vftbl);
                fs.write(R);
                fs.write(G);
                fs.write(B);
        }


        void bCFloatColor::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                fs << indent << "void * __vftbl // " << __vftbl << newLine;
                fs << indent << "GEFloat R // " << R << newLine;
                fs << indent << "GEFloat G // " << G << newLine;
                fs << indent << "GEFloat B // " << B << newLine;
        }


        rmU32 bCFloatColor::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return (sizeof(rmU32) + sizeof(rmF32) * 3);
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // bCEulerAngles
        ////////////////////////////////////////////////////////////////////////////////////


        bCEulerAngles::bCEulerAngles(void)
        {
                setClassName("bCEulerAngles");
                simple = false;
        }


        bCEulerAngles::bCEulerAngles(const bCEulerAngles & src) : Pitch(src.Pitch), Roll(src.Roll), Yaw(src.Yaw) {}


        bCEulerAngles::~bCEulerAngles(void) {}


        WrittenClassBase * bCEulerAngles::clone(void) const
        {
                return new bCEulerAngles(*this);
        }


        void bCEulerAngles::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                fs.read(Pitch);
                fs.read(Roll);
                fs.read(Yaw);
        }


        void bCEulerAngles::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                fs.write(Pitch);
                fs.write(Roll);
                fs.write(Yaw);
        }


        void bCEulerAngles::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                fs << indent << "GEFloat Pitch // " << Pitch << newLine;
                fs << indent << "GEFloat Roll // " << Roll << newLine;
                fs << indent << "GEFloat Yaw // " << Yaw << newLine;
        }


        rmU32 bCEulerAngles::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return (sizeof(rmF32) * 3);
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // bCVector2
        ////////////////////////////////////////////////////////////////////////////////////


        bCVector2::bCVector2(void)
        {
                setClassName("bCVector2");
                simple = false;
        }


        bCVector2::bCVector2(const bCVector2 & src) : X(src.X), Y(src.Y) {}


        bCVector2::~bCVector2(void) {}


        WrittenClassBase * bCVector2::clone(void) const
        {
                return new bCVector2(*this);
        }


        void bCVector2::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                fs.read(X);
                fs.read(Y);
        }


        void bCVector2::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                fs.write(X);
                fs.write(Y);
        }


        void bCVector2::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                fs << indent << "GEFloat X // " << X << newLine;
                fs << indent << "GEFloat Y // " << Y << newLine;
        }


        rmU32 bCVector2::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return (sizeof(rmF32) * 2);
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // bCRect
        ////////////////////////////////////////////////////////////////////////////////////


        bCRect::bCRect(void)
        {
                setClassName("bCRect");
                simple = false;
        }


        bCRect::bCRect(const bCRect & src) : Unknown1(src.Unknown1), Unknown2(src.Unknown2), Unknown3(src.Unknown3), Unknown4(src.Unknown4) {}


        bCRect::~bCRect(void) {}


        WrittenClassBase * bCRect::clone(void) const
        {
                return new bCRect(*this);
        }


        void bCRect::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                fs.read(Unknown1);
                fs.read(Unknown2);
                fs.read(Unknown3);
                fs.read(Unknown4);
        }


        void bCRect::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                fs.write(Unknown1);
                fs.write(Unknown2);
                fs.write(Unknown3);
                fs.write(Unknown4);
        }


        void bCRect::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                fs << indent << "GEU32 Unknown1 // " << Unknown1 << newLine;
                fs << indent << "GEU32 Unknown2 // " << Unknown2 << newLine;
                fs << indent << "GEU32 Unknown3 // " << Unknown3 << newLine;
                fs << indent << "GEU32 Unknown4 // " << Unknown4 << newLine;
        }


        rmU32 bCRect::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return (sizeof(rmU32) * 4);
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // bCAccessorPropertyObject
        ////////////////////////////////////////////////////////////////////////////////////


        bCAccessorPropertyObject::bCAccessorPropertyObject(void)
        {
                setClassName("bCAccessorPropertyObject");
                simple = false;
                ClassData = 0;
        }


        bCAccessorPropertyObject::bCAccessorPropertyObject(const bCAccessorPropertyObject & src) : Version(src.Version), HasPropertyObject(src.HasPropertyObject), Reserved1(src.Reserved1), Reserved2(src.Reserved2), ClassName(src.ClassName), Reserved3(src.Reserved3), Reserved4(src.Reserved4), ClassVersion(src.ClassVersion), Reserved5(src.Reserved5), DataSize(src.DataSize), Version_(src.Version_), Count(src.Count), Properties(src.Properties), ClassData((src.ClassData == 0) ? 0 : src.ClassData->clone()) {}


        bCAccessorPropertyObject::~bCAccessorPropertyObject(void)
        {
                        delete ClassData;
        }


        WrittenClassBase * bCAccessorPropertyObject::clone(void) const
        {
                return new bCAccessorPropertyObject(*this);
        }


        void bCAccessorPropertyObject::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                fs.read(Version);
                HasPropertyObject = (fs.readByte() != 0);


                if (HasPropertyObject) {
                        fs.read(Reserved1);
                        Reserved2 = (fs.readByte() != 0);
                        ClassName.read(fs);
                        fs.read(Reserved3);
                        Reserved4 = (fs.readByte() != 0);
                        fs.read(ClassVersion);
                        fs.read(Reserved5);
                        fs.read(DataSize);


                        fs.read(Version_);
                        fs.read(Count);
                        Properties.resize(Count);
                        for (list<bCAccessorProperty>::iterator i = Properties.begin(), ie = Properties.end(); i != ie; ++i) {
                                i->read(fs);
                        }


                        DummyClassManager::assignDummyClass(ClassData, ClassName.Value);
                        ClassData->read(fs);
                }
        }


        void bCAccessorPropertyObject::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);

                fs.write(Version);
                fs.writeByte(HasPropertyObject);


                if (HasPropertyObject) {
                        fs.write(Reserved1);
                        fs.writeByte(Reserved2);
                        ClassName.write(fs);
                        fs.write(Reserved3);
                        fs.writeByte(Reserved4);
                        fs.write(ClassVersion);
                        fs.write(Reserved5);
                        fs.write(getWriteSize() - (sizeof(rmU16) * 5 + sizeof(rmBool) * 3 + sizeof(rmU32) * 1 + ClassName.getWriteSize())); // DataSize.


                        fs.write(Version_);
                        fs.write(Count);
                        for (list<bCAccessorProperty>::const_iterator i = Properties.begin(), ie = Properties.end(); i != ie; ++i) {
                                i->write(fs);
                        }


                        ClassData->write(fs);
                }
        }


        void bCAccessorPropertyObject::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                fs << indent << "GEU16 Version // " << Version << newLine;
                fs << indent << "GEBool HasPropertyObject // " << HasPropertyObject << newLine;


                if (HasPropertyObject) {
                        fs << indent << "GEU16 Reserved1 // " << Reserved1 << newLine;
                        fs << indent << "GEBool Reserved2 // " << Reserved2 << newLine;
                        DummyClassManager::asciiWriteAsMember(fs, indent, &ClassName, "ClassName");
                        fs << indent << "GEU16 Reserved3 // " << Reserved3 << newLine;
                        fs << indent << "GEBool Reserved4 // " << Reserved4 << newLine;
                        fs << indent << "GEU16 ClassVersion // " << ClassVersion << newLine;
                        fs << indent << "GEU16 Reserved5 // " << Reserved5 << newLine;
                        fs << indent << "GEU32 DataSize // " << DataSize << newLine;


                        fs << indent << "<unknown type> Data {" << newLine;
                        {
                                string indent2 = string("\t").append(indent);


                                fs << indent2 << "<unknown type> PropertyData {" << newLine;
                                {
                                        string indent3 = string("\t").append(indent2);


                                        fs << indent3 << "GEU16 Version // " << Version_ << newLine;
                                        fs << indent3 << "GEU32 Count // " << Count << newLine;
                                        fs << indent3 << "bCAccessorProperty <Properties>[" << Count << "] {" << newLine;
                                        {
                                                rmChar buffer[5];
                                                rmU32 i2 = 0;
                                                for (list<bCAccessorProperty>::const_iterator i = Properties.begin(), ie = Properties.end(); i != ie; ++i) {
                                                        DummyClassManager::asciiWriteAsMember(fs, string("\t").append(indent3), &(*i), string("<Properties>[").append(_itoa(i2++, buffer, 10)).append("]").c_str());
                                                }
                                        }
                                        fs << indent3 << "}" << newLine;
                                }
                                fs << indent2 << "}" << newLine;


                                DummyClassManager::asciiWriteAsMember(fs, indent2, ClassData, "ClassData");
                        }
                        fs << indent << "}" << newLine;
                }
        }


        rmU32 bCAccessorPropertyObject::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                rmU32 result = (sizeof(rmU16) + sizeof(rmBool));
                if (HasPropertyObject) {
                        result += (sizeof(rmU16) * 5 + sizeof(rmBool) * 2 + sizeof(rmU32) * 2);
                        result += (ClassName.getWriteSize() + ClassData->getWriteSize());
                        for (list<bCAccessorProperty>::const_iterator i = Properties.begin(), ie = Properties.end(); i != ie; ++i) {
                                result += i->getWriteSize();
                        }
                }
                return result;
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // eCMaterialResource2
        ////////////////////////////////////////////////////////////////////////////////////


        eCMaterialResource2::eCMaterialResource2(void)
        {
                setClassName("eCMaterialResource2");
                simple = false;
        }


        eCMaterialResource2::eCMaterialResource2(const eCMaterialResource2 & src) : Version(src.Version) {}


        eCMaterialResource2::~eCMaterialResource2(void) {}


        WrittenClassBase * eCMaterialResource2::clone(void) const
        {
                return new eCMaterialResource2(*this);
        }


        void eCMaterialResource2::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                fs.read(Version);
        }


        void eCMaterialResource2::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                fs.write(Version);
        }


        void eCMaterialResource2::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                fs << indent << "GEU16 Version // " << Version << newLine;
        }


        rmU32 eCMaterialResource2::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return sizeof(rmU16);
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // eCShaderEllementBase
        ////////////////////////////////////////////////////////////////////////////////////


        eCShaderEllementBase::eCShaderEllementBase(void)
        {
                setClassName("eCShaderEllementBase");
                simple = false;
        }


        eCShaderEllementBase::eCShaderEllementBase(const eCShaderEllementBase & src) : Version(src.Version), Token(src.Token), EditorLayout(src.EditorLayout), Unknown(src.Unknown) {}


        eCShaderEllementBase::~eCShaderEllementBase(void) {}


        WrittenClassBase * eCShaderEllementBase::clone(void) const
        {
                return new eCShaderEllementBase(*this);
        }


        void eCShaderEllementBase::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);


                fs.read(Version);
                Token.read(fs);
                EditorLayout.read(fs);


                if (Version >= 2) {
                        Unknown = (fs.readByte() != 0);
                }
        }


        void eCShaderEllementBase::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);


                fs.write(Version);
                Token.write(fs);
                EditorLayout.write(fs);


                if (Version >= 2) {
                        fs.writeByte(Unknown);
                }
        }


        void eCShaderEllementBase::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);


                fs << indent << "GEU16 Version // " << Version << newLine;
                DummyClassManager::asciiWriteAsMember(fs, indent, &Token, "Token");
                DummyClassManager::asciiWriteAsMember(fs, indent, &EditorLayout, "EditorLayout");


                if (Version >= 2) {
                        fs << indent << "GEBool Unknown // " << Unknown << newLine;
                }
        }


        rmU32 eCShaderEllementBase::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();


                rmU32 result = (sizeof(rmU16) + Token.getWriteSize() + EditorLayout.getWriteSize());
                if (Version >= 2) {
                        result += sizeof(rmBool);
                }


                return result;
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // eCShaderBase
        ////////////////////////////////////////////////////////////////////////////////////


        eCShaderBase::eCShaderBase(void)
        {
                setClassName("eCShaderBase");
                simple = false;
        }


        eCShaderBase::eCShaderBase(const eCShaderBase & src) : Version(src.Version), Inherited(src.Inherited), ShaderElementCount(src.ShaderElementCount)
        {
                ShaderElements.resize(src.ShaderElements.size());
                list<bCAccessorPropertyObject *>::const_iterator j = src.ShaderElements.begin();
                for (list<bCAccessorPropertyObject *>::iterator i = ShaderElements.begin(), ie = ShaderElements.end(); i != ie; ++i) {
                        *i = new bCAccessorPropertyObject(**j);
                }
        }


        eCShaderBase::~eCShaderBase(void)
        {
                for (list<bCAccessorPropertyObject *>::iterator i = ShaderElements.begin(), ie = ShaderElements.end(); i != ie; ++i) {
                        delete (*i);
                }
        }


        WrittenClassBase * eCShaderBase::clone(void) const
        {
                return new eCShaderBase(*this);
        }


        void eCShaderBase::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);


                fs.read(Version);
                Inherited.read(fs);
                fs.read(ShaderElementCount);


                ShaderElements.resize(ShaderElementCount);
                for (list<bCAccessorPropertyObject *>::iterator i = ShaderElements.begin(), ie = ShaderElements.end(); i != ie; ++i) {
                        (*i) = new bCAccessorPropertyObject;
                        (*i)->read(fs);
                }
        }


        void eCShaderBase::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);


                fs.write(Version);
                Inherited.write(fs);
                fs.write(ShaderElementCount);


                for (list<bCAccessorPropertyObject *>::const_iterator i = ShaderElements.begin(), ie = ShaderElements.end(); i != ie; ++i) {
                        (*i)->write(fs);
                }
        }


        void eCShaderBase::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);


                fs << indent << "GEU16 Version // " << Version << newLine;
                DummyClassManager::asciiWriteAsMember(fs, indent, &Inherited, "Inherited");
                fs << indent << "GEU32 ShaderElementCount // " << ShaderElementCount << newLine;


                fs << indent << "bCAccessorPropertyObject <ShaderElements> {" << newLine;
                {
                        rmChar buffer[5];
                        rmU32 i2 = 0;
                        for (list<bCAccessorPropertyObject *>::const_iterator i = ShaderElements.begin(), ie = ShaderElements.end(); i != ie; ++i) {
                                DummyClassManager::asciiWriteAsMember(fs, string("\t").append(indent), *i, string("<ShaderElements>[").append(_itoa(i2++, buffer, 10)).append("]").c_str());
                        }
                }
                fs << indent << "}" << newLine;
        }


        rmU32 eCShaderBase::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();


                rmU32 result = (sizeof(rmU16) + sizeof(rmU32) + Inherited.getWriteSize());
                for (list<bCAccessorPropertyObject *>::const_iterator i = ShaderElements.begin(), ie = ShaderElements.end(); i != ie; ++i) {
                        result += (*i)->getWriteSize();
                }


                return result;
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // eCShaderDefault
        ////////////////////////////////////////////////////////////////////////////////////


        eCShaderDefault::eCShaderDefault(void)
        {
                setClassName("eCShaderDefault");
                simple = false;
        }


        eCShaderDefault::eCShaderDefault(const eCShaderDefault & src) : Version(src.Version), ColorSrcDiffuse(src.ColorSrcDiffuse), ColorSrcOpacity(src.ColorSrcOpacity), ColorSrcSelfIllumination(src.ColorSrcSelfIllumination), ColorSrcSpecular(src.ColorSrcSpecular), ColorSrcSpecularPower(src.ColorSrcSpecularPower), ColorSrcNormal(src.ColorSrcNormal), ColorSrcDistortion(src.ColorSrcDistortion), ColorSrcLightEmission(src.ColorSrcLightEmission), Inherited(src.Inherited) {}


        eCShaderDefault::~eCShaderDefault(void) {}


        WrittenClassBase * eCShaderDefault::clone(void) const
        {
                return new eCShaderDefault(*this);
        }


        void eCShaderDefault::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);


                fs.read(Version);


                ColorSrcDiffuse.read(fs);
                ColorSrcOpacity.read(fs);
                ColorSrcSelfIllumination.read(fs);
                ColorSrcSpecular.read(fs);
                ColorSrcSpecularPower.read(fs);
                ColorSrcNormal.read(fs);
                if (Version >= 2) {
                        ColorSrcDistortion.read(fs);
                }
                if (Version >= 3) {
                        ColorSrcLightEmission.read(fs);
                }


                Inherited.read(fs);
        }


        void eCShaderDefault::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);


                fs.write(Version);


                ColorSrcDiffuse.write(fs);
                ColorSrcOpacity.write(fs);
                ColorSrcSelfIllumination.write(fs);
                ColorSrcSpecular.write(fs);
                ColorSrcSpecularPower.write(fs);
                ColorSrcNormal.write(fs);
                if (Version >= 2) {
                        ColorSrcDistortion.write(fs);
                }
                if (Version >= 3) {
                        ColorSrcLightEmission.write(fs);
                }


                Inherited.write(fs);
        }


        void eCShaderDefault::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);


                fs << indent << "GEU16 Version // " << Version << newLine;


                DummyClassManager::asciiWriteAsMember(fs, indent, &ColorSrcDiffuse, "ColorSrcDiffuse");
                DummyClassManager::asciiWriteAsMember(fs, indent, &ColorSrcOpacity, "ColorSrcOpacity");
                DummyClassManager::asciiWriteAsMember(fs, indent, &ColorSrcSelfIllumination, "ColorSrcSelfIllumination");
                DummyClassManager::asciiWriteAsMember(fs, indent, &ColorSrcSpecular, "ColorSrcSpecular");
                DummyClassManager::asciiWriteAsMember(fs, indent, &ColorSrcSpecularPower, "ColorSrcSpecularPower");
                DummyClassManager::asciiWriteAsMember(fs, indent, &ColorSrcNormal, "ColorSrcNormal");
                if (Version >= 2) {
                        DummyClassManager::asciiWriteAsMember(fs, indent, &ColorSrcDistortion, "ColorSrcDistortion");
                }
                if (Version >= 3) {
                        DummyClassManager::asciiWriteAsMember(fs, indent, &ColorSrcLightEmission, "ColorSrcLightEmission");
                }


                DummyClassManager::asciiWriteAsMember(fs, indent, &Inherited, "Inherited");
        }


        rmU32 eCShaderDefault::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();


                rmU32 result = (sizeof(rmU16) + ColorSrcDiffuse.getWriteSize() + ColorSrcOpacity.getWriteSize() + ColorSrcSelfIllumination.getWriteSize()
                        + ColorSrcSpecular.getWriteSize() + ColorSrcSpecularPower.getWriteSize() + ColorSrcNormal.getWriteSize() + Inherited.getWriteSize());
                if (Version >= 2) {
                        result += ColorSrcDistortion.getWriteSize();
                }
                if (Version >= 3) {
                        result += ColorSrcLightEmission.getWriteSize();
                }


                return result;
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // eCColorSrcProxy
        ////////////////////////////////////////////////////////////////////////////////////


        eCColorSrcProxy::eCColorSrcProxy(void)
        {
                setClassName("eCColorSrcProxy");
                simple = false;
                ColorComponent = new EnumValue("eEShaderColorSrcComponent");
                ColorModifier = new EnumValue("eEShaderColorModifier");
        }


        eCColorSrcProxy::eCColorSrcProxy(const eCColorSrcProxy & src) : ColorComponent(new EnumValue(*src.ColorComponent)), ColorModifier(new EnumValue(*src.ColorModifier)), Token(src.Token) {}


        eCColorSrcProxy::~eCColorSrcProxy(void)
        {
                delete ColorComponent;
                delete ColorModifier;
        }


        WrittenClassBase * eCColorSrcProxy::clone(void) const
        {
                return new eCColorSrcProxy(*this);
        }


        void eCColorSrcProxy::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);


                fs.read(Version);
                if (Version >= 666) {
                        ColorComponent->read(fs);
                        ColorModifier->read(fs);
                }


                Token.read(fs);
        }


        void eCColorSrcProxy::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);


                fs.write(Version);
                if (Version >= 666) {
                        ColorComponent->write(fs);
                        ColorModifier->write(fs);
                }


                Token.write(fs);
        }


        void eCColorSrcProxy::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);


                fs << indent << "GEU32 Version // " << Version << newLine;
                if (Version >= 666) {
                        DummyClassManager::asciiWriteAsMember(fs, indent, ColorComponent, "ColorComponent");
                        DummyClassManager::asciiWriteAsMember(fs, indent, ColorModifier, "ColorModifier");
                }


                DummyClassManager::asciiWriteAsMember(fs, indent, &Token, "Token");
        }


        rmU32 eCColorSrcProxy::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();


                rmU32 result = (sizeof(rmU32) + Token.getWriteSize());
                if (Version >= 666) {
                        result += (ColorComponent->getWriteSize() + ColorModifier->getWriteSize());
                }


                return result;
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // eCColorScale
        ////////////////////////////////////////////////////////////////////////////////////


        eCColorScale::eCColorScale(void)
        {
                setClassName("eCColorScale");
                simple = false;
        }


        eCColorScale::eCColorScale(const eCColorScale & src) : Version(src.Version), ArraySize(src.ArraySize), ItemCount(src.ItemCount), Items(src.Items) {}


        eCColorScale::~eCColorScale(void) {}


        WrittenClassBase * eCColorScale::clone(void) const
        {
                return new eCColorScale(*this);
        }


        void eCColorScale::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);


                fs.read(Version);
                fs.read(ArraySize);
                fs.read(ItemCount);


                if (ArraySize != 0) {
                        Items.resize(ItemCount);
                        for (list<ColorScaleItem>::iterator i = Items.begin(), ie = Items.end(); i != ie; ++i) {
                                fs.read(i->Time);
                                fs.read(i->Red);
                                fs.read(i->Green);
                                fs.read(i->Blue);
                                fs.read(i->Alpha);
                        }
                }
        }


        void eCColorScale::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);


                fs.write(Version);
                fs.write(ArraySize);
                fs.write(ItemCount);


                if (ArraySize != 0) {
                        for (list<ColorScaleItem>::const_iterator i = Items.begin(), ie = Items.end(); i != ie; ++i) {
                                fs.write(i->Time);
                                fs.write(i->Red);
                                fs.write(i->Green);
                                fs.write(i->Blue);
                                fs.write(i->Alpha);
                        }
                }
        }


        void eCColorScale::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);


                fs << indent << "GEFl Version // " << Version << newLine;
                fs << indent << "GEU32 ArraySize // " << ArraySize << newLine;
                fs << indent << "GEU32 ItemCount // " << ItemCount << newLine;


                if (ArraySize != 0) {
                        rmChar buffer[5];
                        fs << indent << "<unknown type> Items[" << _itoa(ItemCount, buffer, 10) << "] {" << newLine;
                        {
                                string indent2 = string("\t").append(indent);


                                rmU32 i2 = 0;
                                for (list<ColorScaleItem>::const_iterator i = Items.begin(), ie = Items.end(); i != ie; ++i) {
                                        fs << indent2 << "<unknown type> Items[" << _itoa(i2++, buffer, 10) << "] {" << newLine;
                                        {
                                                string indent3 = string("\t").append(indent2);


                                                fs << indent3 << "GEFloat Time // " << i->Time << newLine;
                                                fs << indent3 << "GEFloat Red // " << i->Red << newLine;
                                                fs << indent3 << "GEFloat Green // " << i->Green << newLine;
                                                fs << indent3 << "GEFloat Blue // " << i->Blue << newLine;
                                                fs << indent3 << "GEFloat Alpha // " << i->Alpha << newLine;
                                        }
                                        fs << indent2 << "}" << newLine;
                                }
                        }
                        fs << indent << "}" << newLine;
                }
        }


        rmU32 eCColorScale::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return (sizeof(rmU32) * 3 + ArraySize);
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // eCColorSrcBase
        ////////////////////////////////////////////////////////////////////////////////////


        eCColorSrcBase::eCColorSrcBase(void)
        {
                setClassName("eCColorSrcBase");
                simple = false;
        }


        eCColorSrcBase::eCColorSrcBase(const eCColorSrcBase & src) : Version(src.Version), Inherited(src.Inherited) {}


        eCColorSrcBase::~eCColorSrcBase(void) {}


        WrittenClassBase * eCColorSrcBase::clone(void) const
        {
                return new eCColorSrcBase(*this);
        }


        void eCColorSrcBase::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                fs.read(Version);
                Inherited.read(fs);
        }


        void eCColorSrcBase::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                fs.write(Version);
                Inherited.write(fs);
        }


        void eCColorSrcBase::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                fs << indent << "GEU16 Version // " << Version << newLine;
                DummyClassManager::asciiWriteAsMember(fs, indent, &Inherited, "Inherited");
        }


        rmU32 eCColorSrcBase::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return (sizeof(rmU16) + Inherited.getWriteSize());
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // eCColorSrcBlend
        ////////////////////////////////////////////////////////////////////////////////////


        eCColorSrcBlend::eCColorSrcBlend(void)
        {
                setClassName("eCColorSrcBlend");
                simple = false;
        }


        eCColorSrcBlend::eCColorSrcBlend(const eCColorSrcBlend & src) : Version(src.Version), ColorSrc1(src.ColorSrc1), ColorSrc2(src.ColorSrc2), BlendSrc(src.BlendSrc), Inherited(src.Inherited) {}


        eCColorSrcBlend::~eCColorSrcBlend(void) {}


        WrittenClassBase * eCColorSrcBlend::clone(void) const
        {
                return new eCColorSrcBlend(*this);
        }


        void eCColorSrcBlend::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                fs.read(Version);
                ColorSrc1.read(fs);
                ColorSrc2.read(fs);
                BlendSrc.read(fs);
                Inherited.read(fs);
        }


        void eCColorSrcBlend::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                fs.write(Version);
                ColorSrc1.write(fs);
                ColorSrc2.write(fs);
                BlendSrc.write(fs);
                Inherited.write(fs);
        }


        void eCColorSrcBlend::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                fs << indent << "GEU16 Version // " << Version << newLine;
                DummyClassManager::asciiWriteAsMember(fs, indent, &ColorSrc1, "ColorSrc1");
                DummyClassManager::asciiWriteAsMember(fs, indent, &ColorSrc2, "ColorSrc2");
                DummyClassManager::asciiWriteAsMember(fs, indent, &BlendSrc, "BlendSrc");
                DummyClassManager::asciiWriteAsMember(fs, indent, &Inherited, "Inherited");
        }


        rmU32 eCColorSrcBlend::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return (sizeof(rmU16) + ColorSrc1.getWriteSize() + ColorSrc2.getWriteSize() + BlendSrc.getWriteSize() + Inherited.getWriteSize());
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // eCColorSrcCamDistance
        ////////////////////////////////////////////////////////////////////////////////////


        eCColorSrcCamDistance::eCColorSrcCamDistance(void)
        {
                setClassName("eCColorSrcCamDistance");
                simple = false;
        }


        eCColorSrcCamDistance::eCColorSrcCamDistance(const eCColorSrcCamDistance & src) : Version(src.Version), Inherited(src.Inherited) {}


        eCColorSrcCamDistance::~eCColorSrcCamDistance(void) {}


        WrittenClassBase * eCColorSrcCamDistance::clone(void) const
        {
                return new eCColorSrcCamDistance(*this);
        }


        void eCColorSrcCamDistance::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                fs.read(Version);
                Inherited.read(fs);
        }


        void eCColorSrcCamDistance::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                fs.write(Version);
                Inherited.write(fs);
        }


        void eCColorSrcCamDistance::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                fs << indent << "GEU16 Version // " << Version << newLine;
                DummyClassManager::asciiWriteAsMember(fs, indent, &Inherited, "Inherited");
        }


        rmU32 eCColorSrcCamDistance::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return (sizeof(rmU16) + Inherited.getWriteSize());
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // eCColorSrcCamObstacle
        ////////////////////////////////////////////////////////////////////////////////////


        eCColorSrcCamObstacle::eCColorSrcCamObstacle(void)
        {
                setClassName("eCColorSrcCamObstacle");
                simple = false;
        }


        eCColorSrcCamObstacle::eCColorSrcCamObstacle(const eCColorSrcCamObstacle & src) : Version(src.Version), Inherited(src.Inherited) {}


        eCColorSrcCamObstacle::~eCColorSrcCamObstacle(void) {}


        WrittenClassBase * eCColorSrcCamObstacle::clone(void) const
        {
                return new eCColorSrcCamObstacle(*this);
        }


        void eCColorSrcCamObstacle::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                fs.read(Version);
                Inherited.read(fs);
        }


        void eCColorSrcCamObstacle::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                fs.write(Version);
                Inherited.write(fs);
        }


        void eCColorSrcCamObstacle::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                fs << indent << "GEU16 Version // " << Version << newLine;
                DummyClassManager::asciiWriteAsMember(fs, indent, &Inherited, "Inherited");
        }


        rmU32 eCColorSrcCamObstacle::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return (sizeof(rmU16) + Inherited.getWriteSize());
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // eCColorSrcCombiner
        ////////////////////////////////////////////////////////////////////////////////////


        eCColorSrcCombiner::eCColorSrcCombiner(void)
        {
                setClassName("eCColorSrcCombiner");
                simple = false;
        }


        eCColorSrcCombiner::eCColorSrcCombiner(const eCColorSrcCombiner & src) : Version(src.Version), ColorSrc1(src.ColorSrc1), ColorSrc2(src.ColorSrc2), Inherited(src.Inherited) {}


        eCColorSrcCombiner::~eCColorSrcCombiner(void) {}


        WrittenClassBase * eCColorSrcCombiner::clone(void) const
        {
                return new eCColorSrcCombiner(*this);
        }


        void eCColorSrcCombiner::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                fs.read(Version);
                ColorSrc1.read(fs);
                ColorSrc2.read(fs);
                Inherited.read(fs);
        }


        void eCColorSrcCombiner::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                fs.write(Version);
                ColorSrc1.write(fs);
                ColorSrc2.write(fs);
                Inherited.write(fs);
        }


        void eCColorSrcCombiner::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                fs << indent << "GEU16 Version // " << Version << newLine;
                DummyClassManager::asciiWriteAsMember(fs, indent, &ColorSrc1, "ColorSrc1");
                DummyClassManager::asciiWriteAsMember(fs, indent, &ColorSrc2, "ColorSrc2");
                DummyClassManager::asciiWriteAsMember(fs, indent, &Inherited, "Inherited");
        }


        rmU32 eCColorSrcCombiner::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return (sizeof(rmU16) + ColorSrc1.getWriteSize() + ColorSrc2.getWriteSize() + Inherited.getWriteSize());
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // eCColorSrcConstant
        ////////////////////////////////////////////////////////////////////////////////////


        eCColorSrcConstant::eCColorSrcConstant(void)
        {
                setClassName("eCColorSrcConstant");
                simple = false;
        }


        eCColorSrcConstant::eCColorSrcConstant(const eCColorSrcConstant & src) : Version(src.Version), Inherited(src.Inherited) {}


        eCColorSrcConstant::~eCColorSrcConstant(void) {}


        WrittenClassBase * eCColorSrcConstant::clone(void) const
        {
                return new eCColorSrcConstant(*this);
        }


        void eCColorSrcConstant::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                fs.read(Version);
                Inherited.read(fs);
        }


        void eCColorSrcConstant::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                fs.write(Version);
                Inherited.write(fs);
        }


        void eCColorSrcConstant::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                fs << indent << "GEU16 Version // " << Version << newLine;
                DummyClassManager::asciiWriteAsMember(fs, indent, &Inherited, "Inherited");
        }


        rmU32 eCColorSrcConstant::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return (sizeof(rmU16) + Inherited.getWriteSize());
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // eCColorSrcConstantAnimation
        ////////////////////////////////////////////////////////////////////////////////////


        eCColorSrcConstantAnimation::eCColorSrcConstantAnimation(void)
        {
                setClassName("eCColorSrcConstantAnimation");
                simple = false;
        }


        eCColorSrcConstantAnimation::eCColorSrcConstantAnimation(const eCColorSrcConstantAnimation & src) : Version(src.Version), ColorScale(src.ColorScale), Inherited(src.Inherited) {}


        eCColorSrcConstantAnimation::~eCColorSrcConstantAnimation(void) {}


        WrittenClassBase * eCColorSrcConstantAnimation::clone(void) const
        {
                return new eCColorSrcConstantAnimation(*this);
        }


        void eCColorSrcConstantAnimation::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                fs.read(Version);
                ColorScale.read(fs);
                Inherited.read(fs);
        }


        void eCColorSrcConstantAnimation::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                fs.write(Version);
                ColorScale.write(fs);
                Inherited.write(fs);
        }


        void eCColorSrcConstantAnimation::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                fs << indent << "GEU16 Version // " << Version << newLine;
                DummyClassManager::asciiWriteAsMember(fs, indent, &ColorScale, "ColorScale");
                DummyClassManager::asciiWriteAsMember(fs, indent, &Inherited, "Inherited");
        }


        rmU32 eCColorSrcConstantAnimation::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return (sizeof(rmU16) + ColorScale.getWriteSize() + Inherited.getWriteSize());
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // eCColorSrcConstantSwitch
        ////////////////////////////////////////////////////////////////////////////////////


        eCColorSrcConstantSwitch::eCColorSrcConstantSwitch(void)
        {
                setClassName("eCColorSrcConstantSwitch");
                simple = false;
        }


        eCColorSrcConstantSwitch::eCColorSrcConstantSwitch(const eCColorSrcConstantSwitch & src) : Version(src.Version), Inherited(src.Inherited) {}


        eCColorSrcConstantSwitch::~eCColorSrcConstantSwitch(void) {}


        WrittenClassBase * eCColorSrcConstantSwitch::clone(void) const
        {
                return new eCColorSrcConstantSwitch(*this);
        }


        void eCColorSrcConstantSwitch::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                fs.read(Version);
                Inherited.read(fs);
        }


        void eCColorSrcConstantSwitch::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                fs.write(Version);
                Inherited.write(fs);
        }


        void eCColorSrcConstantSwitch::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                fs << indent << "GEU16 Version // " << Version << newLine;
                DummyClassManager::asciiWriteAsMember(fs, indent, &Inherited, "Inherited");
        }


        rmU32 eCColorSrcConstantSwitch::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return (sizeof(rmU16) + Inherited.getWriteSize());
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // eCColorSrcCubeSampler
        ////////////////////////////////////////////////////////////////////////////////////


        eCColorSrcCubeSampler::eCColorSrcCubeSampler(void)
        {
                setClassName("eCColorSrcCubeSampler");
                simple = false;
        }


        eCColorSrcCubeSampler::eCColorSrcCubeSampler(const eCColorSrcCubeSampler & src) : Version(src.Version), Inherited(src.Inherited) {}


        eCColorSrcCubeSampler::~eCColorSrcCubeSampler(void) {}


        WrittenClassBase * eCColorSrcCubeSampler::clone(void) const
        {
                return new eCColorSrcCubeSampler(*this);
        }


        void eCColorSrcCubeSampler::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                fs.read(Version);
                Inherited.read(fs);
        }


        void eCColorSrcCubeSampler::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                fs.write(Version);
                Inherited.write(fs);
        }


        void eCColorSrcCubeSampler::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                fs << indent << "GEU16 Version // " << Version << newLine;
                DummyClassManager::asciiWriteAsMember(fs, indent, &Inherited, "Inherited");
        }


        rmU32 eCColorSrcCubeSampler::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return (sizeof(rmU16) + Inherited.getWriteSize());
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // eCColorSrcFresnel
        ////////////////////////////////////////////////////////////////////////////////////


        eCColorSrcFresnel::eCColorSrcFresnel(void)
        {
                setClassName("eCColorSrcFresnel");
                simple = false;
        }


        eCColorSrcFresnel::eCColorSrcFresnel(const eCColorSrcFresnel & src) : Version(src.Version), Inherited(src.Inherited) {}


        eCColorSrcFresnel::~eCColorSrcFresnel(void) {}


        WrittenClassBase * eCColorSrcFresnel::clone(void) const
        {
                return new eCColorSrcFresnel(*this);
        }


        void eCColorSrcFresnel::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                fs.read(Version);
                Inherited.read(fs);
        }


        void eCColorSrcFresnel::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                fs.write(Version);
                Inherited.write(fs);
        }


        void eCColorSrcFresnel::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                fs << indent << "GEU16 Version // " << Version << newLine;
                DummyClassManager::asciiWriteAsMember(fs, indent, &Inherited, "Inherited");
        }


        rmU32 eCColorSrcFresnel::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return (sizeof(rmU16) + Inherited.getWriteSize());
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // eCColorSrcMask
        ////////////////////////////////////////////////////////////////////////////////////


        eCColorSrcMask::eCColorSrcMask(void)
        {
                setClassName("eCColorSrcMask");
                simple = false;
        }


        eCColorSrcMask::eCColorSrcMask(const eCColorSrcMask & src) : Version(src.Version), Color(src.Color), Inherited(src.Inherited) {}


        eCColorSrcMask::~eCColorSrcMask(void) {}


        WrittenClassBase * eCColorSrcMask::clone(void) const
        {
                return new eCColorSrcMask(*this);
        }


        void eCColorSrcMask::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                fs.read(Version);
                Color.read(fs);
                Inherited.read(fs);
        }


        void eCColorSrcMask::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                fs.write(Version);
                Color.write(fs);
                Inherited.write(fs);
        }


        void eCColorSrcMask::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                fs << indent << "GEU16 Version // " << Version << newLine;
                DummyClassManager::asciiWriteAsMember(fs, indent, &Color, "Color");
                DummyClassManager::asciiWriteAsMember(fs, indent, &Inherited, "Inherited");
        }


        rmU32 eCColorSrcMask::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return (sizeof(rmU16) + Color.getWriteSize() + Inherited.getWriteSize());
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // eCColorSrcSampler
        ////////////////////////////////////////////////////////////////////////////////////


        eCColorSrcSampler::eCColorSrcSampler(void)
        {
                setClassName("eCColorSrcSampler");
                simple = false;
        }


        eCColorSrcSampler::eCColorSrcSampler(const eCColorSrcSampler & src) : Version(src.Version), TexCoord(src.TexCoord), Unknown(src.Unknown), Inherited(src.Inherited) {}


        eCColorSrcSampler::~eCColorSrcSampler(void) {}


        WrittenClassBase * eCColorSrcSampler::clone(void) const
        {
                return new eCColorSrcSampler(*this);
        }


        void eCColorSrcSampler::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                fs.read(Version);
                TexCoord.read(fs);
                fs.read(Unknown);
                Inherited.read(fs);
        }


        void eCColorSrcSampler::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                fs.write(Version);
                TexCoord.write(fs);
                fs.write(Unknown);
                Inherited.write(fs);
        }


        void eCColorSrcSampler::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                fs << indent << "GEU16 Version // " << Version << newLine;
                DummyClassManager::asciiWriteAsMember(fs, indent, &TexCoord, "TexCoord");
                fs << indent << "GEU32 Unknown // " << eCColorSrcSampler::Unknown << newLine;
                DummyClassManager::asciiWriteAsMember(fs, indent, &Inherited, "Inherited");
        }


        rmU32 eCColorSrcSampler::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return (sizeof(rmU16) + TexCoord.getWriteSize() + sizeof(rmU32) + Inherited.getWriteSize());
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // eCColorSrcSkydomeSampler
        ////////////////////////////////////////////////////////////////////////////////////


        eCColorSrcSkydomeSampler::eCColorSrcSkydomeSampler(void)
        {
                setClassName("eCColorSrcSkydomeSampler");
                simple = false;
        }


        eCColorSrcSkydomeSampler::eCColorSrcSkydomeSampler(const eCColorSrcSkydomeSampler & src) : Version(src.Version), Inherited(src.Inherited) {}


        eCColorSrcSkydomeSampler::~eCColorSrcSkydomeSampler(void) {}


        WrittenClassBase * eCColorSrcSkydomeSampler::clone(void) const
        {
                return new eCColorSrcSkydomeSampler(*this);
        }


        void eCColorSrcSkydomeSampler::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                fs.read(Version);
                Inherited.read(fs);
        }


        void eCColorSrcSkydomeSampler::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                fs.write(Version);
                Inherited.write(fs);
        }


        void eCColorSrcSkydomeSampler::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                fs << indent << "GEU16 Version // " << Version << newLine;
                DummyClassManager::asciiWriteAsMember(fs, indent, &Inherited, "Inherited");
        }


        rmU32 eCColorSrcSkydomeSampler::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return (sizeof(rmU16) + Inherited.getWriteSize());
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // eCColorSrcVertexColor
        ////////////////////////////////////////////////////////////////////////////////////


        eCColorSrcVertexColor::eCColorSrcVertexColor(void)
        {
                setClassName("eCColorSrcVertexColor");
                simple = false;
        }


        eCColorSrcVertexColor::eCColorSrcVertexColor(const eCColorSrcVertexColor & src) : Version(src.Version), Inherited(src.Inherited) {}


        eCColorSrcVertexColor::~eCColorSrcVertexColor(void) {}


        WrittenClassBase * eCColorSrcVertexColor::clone(void) const
        {
                return new eCColorSrcVertexColor(*this);
        }


        void eCColorSrcVertexColor::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                fs.read(Version);
                Inherited.read(fs);
        }


        void eCColorSrcVertexColor::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                fs.write(Version);
                Inherited.write(fs);
        }


        void eCColorSrcVertexColor::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                fs << indent << "GEU16 Version // " << Version << newLine;
                DummyClassManager::asciiWriteAsMember(fs, indent, &Inherited, "Inherited");
        }


        rmU32 eCColorSrcVertexColor::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return (sizeof(rmU16) + Inherited.getWriteSize());
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // eCShaderLeafs
        ////////////////////////////////////////////////////////////////////////////////////


        eCShaderLeafs::eCShaderLeafs(void)
        {
                setClassName("eCShaderLeafs");
                simple = false;
        }


        eCShaderLeafs::eCShaderLeafs(const eCShaderLeafs & src) : eCShaderDefault(src.eCShaderDefault) {}


        eCShaderLeafs::~eCShaderLeafs(void) {}


        WrittenClassBase * eCShaderLeafs::clone(void) const
        {
                return new eCShaderLeafs(*this);
        }


        void eCShaderLeafs::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                eCShaderDefault.read(fs);
        }


        void eCShaderLeafs::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                eCShaderDefault.write(fs);
        }


        void eCShaderLeafs::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                DummyClassManager::asciiWriteAsMember(fs, indent, &eCShaderDefault, "eCShaderDefault");
        }


        rmU32 eCShaderLeafs::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return (eCShaderDefault.getWriteSize());
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // eCShaderLightStreaks
        ////////////////////////////////////////////////////////////////////////////////////


        eCShaderLightStreaks::eCShaderLightStreaks(void)
        {
                setClassName("eCShaderLightStreaks");
                simple = false;
        }


        eCShaderLightStreaks::eCShaderLightStreaks(const eCShaderLightStreaks & src) : Version(src.Version), ColorSrcDiffuse(src.ColorSrcDiffuse), Inherited(src.Inherited) {}


        eCShaderLightStreaks::~eCShaderLightStreaks(void) {}


        WrittenClassBase * eCShaderLightStreaks::clone(void) const
        {
                return new eCShaderLightStreaks(*this);
        }


        void eCShaderLightStreaks::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                fs.read(Version);
                ColorSrcDiffuse.read(fs);
                Inherited.read(fs);
        }


        void eCShaderLightStreaks::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                fs.write(Version);
                ColorSrcDiffuse.write(fs);
                Inherited.write(fs);
        }


        void eCShaderLightStreaks::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                fs << indent << "GEU16 Version // " << Version << newLine;
                DummyClassManager::asciiWriteAsMember(fs, indent, &ColorSrcDiffuse, "ColorSrcDiffuse");
                DummyClassManager::asciiWriteAsMember(fs, indent, &Inherited, "Inherited");
        }


        rmU32 eCShaderLightStreaks::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return (sizeof(rmU16) + ColorSrcDiffuse.getWriteSize() + Inherited.getWriteSize());
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // eCShaderParticle
        ////////////////////////////////////////////////////////////////////////////////////


        eCShaderParticle::eCShaderParticle(void)
        {
                setClassName("eCShaderParticle");
                simple = false;
        }


        eCShaderParticle::eCShaderParticle(const eCShaderParticle & src) : Version(src.Version), ColorSrcDiffuse(src.ColorSrcDiffuse), Inherited(src.Inherited) {}


        eCShaderParticle::~eCShaderParticle(void) {}


        WrittenClassBase * eCShaderParticle::clone(void) const
        {
                return new eCShaderParticle(*this);
        }


        void eCShaderParticle::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                fs.read(Version);
                ColorSrcDiffuse.read(fs);
                Inherited.read(fs);
        }


        void eCShaderParticle::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                fs.write(Version);
                ColorSrcDiffuse.write(fs);
                Inherited.write(fs);
        }


        void eCShaderParticle::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                fs << indent << "GEU16 Version // " << Version << newLine;
                DummyClassManager::asciiWriteAsMember(fs, indent, &ColorSrcDiffuse, "ColorSrcDiffuse");
                DummyClassManager::asciiWriteAsMember(fs, indent, &Inherited, "Inherited");
        }


        rmU32 eCShaderParticle::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return (sizeof(rmU16) + ColorSrcDiffuse.getWriteSize() + Inherited.getWriteSize());
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // eCShaderSkyDome
        ////////////////////////////////////////////////////////////////////////////////////


        eCShaderSkyDome::eCShaderSkyDome(void)
        {
                setClassName("eCShaderSkyDome");
                simple = false;
        }


        eCShaderSkyDome::eCShaderSkyDome(const eCShaderSkyDome & src) : Version(src.Version), ColorSrcClouds(src.ColorSrcClouds), ColorSrcAbsorbation(src.ColorSrcAbsorbation), Inherited(src.Inherited) {}


        eCShaderSkyDome::~eCShaderSkyDome(void) {}


        WrittenClassBase * eCShaderSkyDome::clone(void) const
        {
                return new eCShaderSkyDome(*this);
        }


        void eCShaderSkyDome::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                fs.read(Version);
                ColorSrcClouds.read(fs);
                ColorSrcAbsorbation.read(fs);
                Inherited.read(fs);
        }


        void eCShaderSkyDome::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                fs.write(Version);
                ColorSrcClouds.write(fs);
                ColorSrcAbsorbation.write(fs);
                Inherited.write(fs);
        }


        void eCShaderSkyDome::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                fs << indent << "GEU16 Version // " << Version << newLine;
                DummyClassManager::asciiWriteAsMember(fs, indent, &ColorSrcClouds, "ColorSrcClouds");
                DummyClassManager::asciiWriteAsMember(fs, indent, &ColorSrcAbsorbation, "ColorSrcAbsorbation");
                DummyClassManager::asciiWriteAsMember(fs, indent, &Inherited, "Inherited");
        }


        rmU32 eCShaderSkyDome::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return (sizeof(rmU16) + ColorSrcClouds.getWriteSize() + ColorSrcAbsorbation.getWriteSize() + Inherited.getWriteSize());
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // eCTexCoordSrcBase
        ////////////////////////////////////////////////////////////////////////////////////


        eCTexCoordSrcBase::eCTexCoordSrcBase(void)
        {
                setClassName("eCTexCoordSrcBase");
                simple = false;
        }


        eCTexCoordSrcBase::eCTexCoordSrcBase(const eCTexCoordSrcBase & src) : Version(src.Version), Inherited(src.Inherited) {}


        eCTexCoordSrcBase::~eCTexCoordSrcBase(void) {}


        WrittenClassBase * eCTexCoordSrcBase::clone(void) const
        {
                return new eCTexCoordSrcBase(*this);
        }


        void eCTexCoordSrcBase::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                fs.read(Version);
                Inherited.read(fs);
        }


        void eCTexCoordSrcBase::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                fs.write(Version);
                Inherited.write(fs);
        }


        void eCTexCoordSrcBase::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                fs << indent << "GEU16 Version // " << Version << newLine;
                DummyClassManager::asciiWriteAsMember(fs, indent, &Inherited, "Inherited");
        }


        rmU32 eCTexCoordSrcBase::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return (sizeof(rmU16) + Inherited.getWriteSize());
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // eCTexCoordSrcBumpOffset
        ////////////////////////////////////////////////////////////////////////////////////


        eCTexCoordSrcBumpOffset::eCTexCoordSrcBumpOffset(void)
        {
                setClassName("eCTexCoordSrcBumpOffset");
                simple = false;
        }


        eCTexCoordSrcBumpOffset::eCTexCoordSrcBumpOffset(const eCTexCoordSrcBumpOffset & src) : Version(src.Version), Height(src.Height), TexCoord(src.TexCoord), Inherited(src.Inherited) {}


        eCTexCoordSrcBumpOffset::~eCTexCoordSrcBumpOffset(void) {}


        WrittenClassBase * eCTexCoordSrcBumpOffset::clone(void) const
        {
                return new eCTexCoordSrcBumpOffset(*this);
        }


        void eCTexCoordSrcBumpOffset::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                fs.read(Version);
                Height.read(fs);
                TexCoord.read(fs);
                Inherited.read(fs);
        }


        void eCTexCoordSrcBumpOffset::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                fs.write(Version);
                Height.write(fs);
                TexCoord.write(fs);
                Inherited.write(fs);
        }


        void eCTexCoordSrcBumpOffset::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                fs << indent << "GEU16 Version // " << Version << newLine;
                DummyClassManager::asciiWriteAsMember(fs, indent, &Height, "Height");
                DummyClassManager::asciiWriteAsMember(fs, indent, &TexCoord, "TexCoord");
                DummyClassManager::asciiWriteAsMember(fs, indent, &Inherited, "Inherited");
        }


        rmU32 eCTexCoordSrcBumpOffset::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return (sizeof(rmU16) + Height.getWriteSize() + TexCoord.getWriteSize() + Inherited.getWriteSize());
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // eCTexCoordSrcColor
        ////////////////////////////////////////////////////////////////////////////////////


        eCTexCoordSrcColor::eCTexCoordSrcColor(void)
        {
                setClassName("eCTexCoordSrcColor");
                simple = false;
        }


        eCTexCoordSrcColor::eCTexCoordSrcColor(const eCTexCoordSrcColor & src) : Version(src.Version), ColorU(src.ColorU), ColorV(src.ColorV), Inherited(src.Inherited) {}


        eCTexCoordSrcColor::~eCTexCoordSrcColor(void) {}


        WrittenClassBase * eCTexCoordSrcColor::clone(void) const
        {
                return new eCTexCoordSrcColor(*this);
        }


        void eCTexCoordSrcColor::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                fs.read(Version);
                ColorU.read(fs);
                ColorV.read(fs);
                Inherited.read(fs);
        }


        void eCTexCoordSrcColor::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                fs.write(Version);
                ColorU.write(fs);
                ColorV.write(fs);
                Inherited.write(fs);
        }


        void eCTexCoordSrcColor::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                fs << indent << "GEU16 Version // " << Version << newLine;
                DummyClassManager::asciiWriteAsMember(fs, indent, &ColorU, "ColorU");
                DummyClassManager::asciiWriteAsMember(fs, indent, &ColorV, "ColorV");
                DummyClassManager::asciiWriteAsMember(fs, indent, &Inherited, "Inherited");
        }


        rmU32 eCTexCoordSrcColor::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return (sizeof(rmU16) + ColorU.getWriteSize() + ColorV.getWriteSize() + Inherited.getWriteSize());
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // eCTexCoordSrcOscillator
        ////////////////////////////////////////////////////////////////////////////////////


        eCTexCoordSrcOscillator::eCTexCoordSrcOscillator(void)
        {
                setClassName("eCTexCoordSrcOscillator");
                simple = false;
        }


        eCTexCoordSrcOscillator::eCTexCoordSrcOscillator(const eCTexCoordSrcOscillator & src) : TexCoord(src.TexCoord), Inherited(src.Inherited) {}


        eCTexCoordSrcOscillator::~eCTexCoordSrcOscillator(void) {}


        WrittenClassBase * eCTexCoordSrcOscillator::clone(void) const
        {
                return new eCTexCoordSrcOscillator(*this);
        }


        void eCTexCoordSrcOscillator::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                TexCoord.read(fs);
                Inherited.read(fs);
        }


        void eCTexCoordSrcOscillator::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                TexCoord.write(fs);
                Inherited.write(fs);
        }


        void eCTexCoordSrcOscillator::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                DummyClassManager::asciiWriteAsMember(fs, indent, &TexCoord, "TexCoord");
                DummyClassManager::asciiWriteAsMember(fs, indent, &Inherited, "Inherited");
        }


        rmU32 eCTexCoordSrcOscillator::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return (TexCoord.getWriteSize() + Inherited.getWriteSize());
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // eCTexCoordSrcProxy
        ////////////////////////////////////////////////////////////////////////////////////


        eCTexCoordSrcProxy::eCTexCoordSrcProxy(void)
        {
                setClassName("eCTexCoordSrcProxy");
                simple = false;
        }


        eCTexCoordSrcProxy::eCTexCoordSrcProxy(const eCTexCoordSrcProxy & src) : VertexTexCoord(src.VertexTexCoord), Token(src.Token) {}


        eCTexCoordSrcProxy::~eCTexCoordSrcProxy(void) {}


        WrittenClassBase * eCTexCoordSrcProxy::clone(void) const
        {
                return new eCTexCoordSrcProxy(*this);
        }


        void eCTexCoordSrcProxy::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                fs.read(VertexTexCoord);
                Token.read(fs);
        }


        void eCTexCoordSrcProxy::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                fs.write(VertexTexCoord);
                Token.write(fs);
        }


        void eCTexCoordSrcProxy::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                fs << indent << "GEU32 VertexTexCoord // " << eCTexCoordSrcProxy::VertexTexCoord << newLine;
                DummyClassManager::asciiWriteAsMember(fs, indent, &Token, "Token");
        }


        rmU32 eCTexCoordSrcProxy::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return (sizeof(rmU32) + Token.getWriteSize());
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // eCTexCoordSrcReflect
        ////////////////////////////////////////////////////////////////////////////////////


        eCTexCoordSrcReflect::eCTexCoordSrcReflect(void)
        {
                setClassName("eCTexCoordSrcReflect");
                simple = false;
        }


        eCTexCoordSrcReflect::eCTexCoordSrcReflect(const eCTexCoordSrcReflect & src) : Inherited(src.Inherited) {}


        eCTexCoordSrcReflect::~eCTexCoordSrcReflect(void) {}


        WrittenClassBase * eCTexCoordSrcReflect::clone(void) const
        {
                return new eCTexCoordSrcReflect(*this);
        }


        void eCTexCoordSrcReflect::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                Inherited.read(fs);
        }


        void eCTexCoordSrcReflect::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                Inherited.write(fs);
        }


        void eCTexCoordSrcReflect::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                DummyClassManager::asciiWriteAsMember(fs, indent, &Inherited, "Inherited");
        }


        rmU32 eCTexCoordSrcReflect::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return (Inherited.getWriteSize());
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // eCTexCoordSrcRotator
        ////////////////////////////////////////////////////////////////////////////////////


        eCTexCoordSrcRotator::eCTexCoordSrcRotator(void)
        {
                setClassName("eCTexCoordSrcRotator");
                simple = false;
        }


        eCTexCoordSrcRotator::eCTexCoordSrcRotator(const eCTexCoordSrcRotator & src) : Version(src.Version), TexCoord(src.TexCoord), Inherited(src.Inherited) {}


        eCTexCoordSrcRotator::~eCTexCoordSrcRotator(void) {}


        WrittenClassBase * eCTexCoordSrcRotator::clone(void) const
        {
                return new eCTexCoordSrcRotator(*this);
        }


        void eCTexCoordSrcRotator::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                fs.read(Version);
                TexCoord.read(fs);
                Inherited.read(fs);
        }


        void eCTexCoordSrcRotator::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                fs.write(Version);
                TexCoord.write(fs);
                Inherited.write(fs);
        }


        void eCTexCoordSrcRotator::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                fs << indent << "GEU16 Version // " << Version << newLine;
                DummyClassManager::asciiWriteAsMember(fs, indent, &TexCoord, "TexCoord");
                DummyClassManager::asciiWriteAsMember(fs, indent, &Inherited, "Inherited");
        }


        rmU32 eCTexCoordSrcRotator::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return (sizeof(rmU16) + TexCoord.getWriteSize() + Inherited.getWriteSize());
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // eCTexCoordSrcScale
        ////////////////////////////////////////////////////////////////////////////////////


        eCTexCoordSrcScale::eCTexCoordSrcScale(void)
        {
                setClassName("eCTexCoordSrcScale");
                simple = false;
        }


        eCTexCoordSrcScale::eCTexCoordSrcScale(const eCTexCoordSrcScale & src) : Version(src.Version), TexCoord(src.TexCoord), Inherited(src.Inherited) {}


        eCTexCoordSrcScale::~eCTexCoordSrcScale(void) {}


        WrittenClassBase * eCTexCoordSrcScale::clone(void) const
        {
                return new eCTexCoordSrcScale(*this);
        }


        void eCTexCoordSrcScale::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                fs.read(Version);
                TexCoord.read(fs);
                Inherited.read(fs);
        }


        void eCTexCoordSrcScale::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                fs.write(Version);
                TexCoord.write(fs);
                Inherited.write(fs);
        }


        void eCTexCoordSrcScale::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                fs << indent << "GEU16 Version // " << Version << newLine;
                DummyClassManager::asciiWriteAsMember(fs, indent, &TexCoord, "TexCoord");
                DummyClassManager::asciiWriteAsMember(fs, indent, &Inherited, "Inherited");
        }


        rmU32 eCTexCoordSrcScale::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return (sizeof(rmU16) + TexCoord.getWriteSize() + Inherited.getWriteSize());
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // eCTexCoordSrcScroller
        ////////////////////////////////////////////////////////////////////////////////////


        eCTexCoordSrcScroller::eCTexCoordSrcScroller(void)
        {
                setClassName("eCTexCoordSrcScroller");
                simple = false;
        }


        eCTexCoordSrcScroller::eCTexCoordSrcScroller(const eCTexCoordSrcScroller & src) : Version(src.Version), TexCoord(src.TexCoord), Inherited(src.Inherited) {}


        eCTexCoordSrcScroller::~eCTexCoordSrcScroller(void) {}


        WrittenClassBase * eCTexCoordSrcScroller::clone(void) const
        {
                return new eCTexCoordSrcScroller(*this);
        }


        void eCTexCoordSrcScroller::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                fs.read(Version);
                TexCoord.read(fs);
                Inherited.read(fs);
        }


        void eCTexCoordSrcScroller::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                fs.write(Version);
                TexCoord.write(fs);
                Inherited.write(fs);
        }


        void eCTexCoordSrcScroller::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                fs << indent << "GEU16 Version // " << Version << newLine;
                DummyClassManager::asciiWriteAsMember(fs, indent, &TexCoord, "TexCoord");
                DummyClassManager::asciiWriteAsMember(fs, indent, &Inherited, "Inherited");
        }


        rmU32 eCTexCoordSrcScroller::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return (sizeof(rmU16) + TexCoord.getWriteSize() + Inherited.getWriteSize());
        }


        ////////////////////////////////////////////////////////////////////////////////////
        // xmatFile
        ////////////////////////////////////////////////////////////////////////////////////


        xmatFile::xmatFile(void)
        {
                setClassName("xmatFile");
                simple = false;
        }


        xmatFile::xmatFile(const xmatFile & src) : Revision(src.Revision), DataRevision(src.DataRevision), PropertyObjectOffset(src.PropertyObjectOffset), PropertyObjectSize(src.PropertyObjectSize), ResourceDataOffset(src.ResourceDataOffset), ResourceDataSize(src.ResourceDataSize), RawModificationTime(src.RawModificationTime), RawFileExtension(src.RawFileExtension), Header(src.Header), Data(src.Data) {}


        xmatFile::~xmatFile(void) {}


        WrittenClassBase * xmatFile::clone(void) const
        {
                return new xmatFile(*this);
        }


        void xmatFile::read(rmInFileStream & fs)
        {
                WrittenClassBase::read(fs);
                fs.readString(Revision, 4);
                fs.readString(DataRevision, 4);
                fs.read(PropertyObjectOffset);
                fs.read(PropertyObjectSize);
                fs.read(ResourceDataOffset);
                fs.read(ResourceDataSize);
                fs.read(RawModificationTime);
                fs.readString(RawFileExtension, 8);
                Header.read(fs);
                Data.read(fs);
        }


        void xmatFile::write(rmOutFileStream & fs) const
        {
                WrittenClassBase::write(fs);
                rmU32 ResourceHeaderSize = (sizeof(rmU32) * 6 + sizeof(rmU64) + sizeof(rmChar) * 8);
                rmU32 HeaderSize = Header.getWriteSize();
                rmU32 DataSize = Data.getWriteSize();


                fs.writeString(Revision);
                fs.writeString(DataRevision);
                fs.write(ResourceHeaderSize);
                fs.write(HeaderSize);
                fs.write(ResourceHeaderSize + HeaderSize);
                fs.write(DataSize);
                fs.write(RawModificationTime);
                fs.writeString(RawFileExtension);
                Header.write(fs);
                Data.write(fs);
        }


        void xmatFile::asciiWrite(rmOutFileStream & fs, string indent) const
        {
                WrittenClassBase::asciiWrite(fs, indent);
                rmU32 ResourceHeaderSize = (sizeof(rmU32) * 6 + sizeof(rmU64) + sizeof(rmChar) * 8);
                rmU32 HeaderSize = Header.getWriteSize();
                rmU32 DataSize = Data.getWriteSize();


                rmU32 RevisionAsNumber = *reinterpret_cast<const rmU32 *>(&Revision[0]);
                rmU32 DataRevisionAsNumber = *reinterpret_cast<const rmU32 *>(&Revision[0]);


                fs << indent << "GEU32 Revision // " << RevisionAsNumber << " (\"" << Revision << "\")" << newLine;
                fs << indent << "GEU32 DataRevision // " << DataRevisionAsNumber << " (\"" << DataRevision << "\")" << newLine;
                fs << indent << "GEU32 PropertyObjectOffset // " << ResourceHeaderSize << newLine;
                fs << indent << "GEU32 ResourceDataOffset // " << (ResourceHeaderSize + HeaderSize) << newLine;
                fs << indent << "GEU32 ResourceDataSize // " << DataSize << newLine;


                rmU64 tmp = (RawModificationTime - 116444736000000000) / 10000000;
                time_t RawModificationTime2 = tmp;
                fs << indent << "SFFFileTime RawModificationTime // " << ctime(&RawModificationTime2) << newLine;
                fs << indent << "GEChar RawFileExtension[8] // " << "\"" << RawFileExtension.c_str() << "\"" << newLine;


                DummyClassManager::asciiWriteAsMember(fs, indent, &Header, "Header");
                DummyClassManager::asciiWriteAsMember(fs, indent, &Data, "Data");
        }


        rmU32 xmatFile::getWriteSize(void) const
        {
                WrittenClassBase::getWriteSize();
                return (sizeof(rmU32) * 4 + sizeof(rmChar) * 4 * 2 + sizeof(rmU64) + sizeof(rmChar) * 8 + Header.getWriteSize() + Data.getWriteSize());
        }


        ////////////////////////////////////////////////////////////////////////////////////
        //
        // assignVarsAsProps / doSpecialPropAssign
        //
        ////////////////////////////////////////////////////////////////////////////////////


        bool HexData::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void HexData::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty("<unknown data format>");
        }


        bool EnumValue::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                ::rmPropertyLinker::assingAsProperty(this, name);
                return true;
        }


        void EnumValue::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(this, "Value");
        }


        bool bCGuid::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void bCGuid::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Guid, "Guid");
                ::rmPropertyLinker::assingAsProperty(&Valid, "Valid");
                ::rmPropertyLinker::assingAsProperty(&Alignment, "Alignment");
        }


        bool GEBool::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void GEBool::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Value, "Value");
        }


        bool GEChar::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void GEChar::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty((rmU8 *)(&Value), "Value"); //
        }


        bool GEInt::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void GEInt::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Value, "Value");
        }


        bool GEFloat::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void GEFloat::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Value, "Value");
        }


        bool bCString::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void bCString::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Value, "Value");
        }


        bool bTPropertyContainer::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void bTPropertyContainer::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Version, "Version", false);
                ::rmPropertyLinker::assingAsProperty(&Value, "Value");
        }


        bool bCAccessorProperty::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void bCAccessorProperty::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Name, "Name", false);
                ::rmPropertyLinker::assingAsProperty(&Type, "Type", false);
                ::rmPropertyLinker::assingAsProperty(&Version, "Version", false);
                ::rmPropertyLinker::assingAsProperty(&Data, "Data");
        }


        bool bCFloatColor::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void bCFloatColor::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&__vftbl, "__vftbl", false);
                ::rmPropertyLinker::assingAsProperty(&R, "R");
                ::rmPropertyLinker::assingAsProperty(&G, "G");
                ::rmPropertyLinker::assingAsProperty(&B, "B");
        }


        bool bCEulerAngles::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void bCEulerAngles::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Pitch, "Pitch");
                ::rmPropertyLinker::assingAsProperty(&Roll, "Roll");
                ::rmPropertyLinker::assingAsProperty(&Yaw, "Yaw");
        }


        bool bCVector2::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void bCVector2::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&X, "X");
                ::rmPropertyLinker::assingAsProperty(&Y, "Y");
        }


        bool bCRect::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void bCRect::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Unknown1, "Unknown1");
                ::rmPropertyLinker::assingAsProperty(&Unknown2, "Unknown2");
                ::rmPropertyLinker::assingAsProperty(&Unknown3, "Unknown3");
                ::rmPropertyLinker::assingAsProperty(&Unknown4, "Unknown4");
        }


        bool bCAccessorPropertyObject::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void bCAccessorPropertyObject::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Version, "Version", false);
                ::rmPropertyLinker::assingAsProperty(&HasPropertyObject, "HasPropertyObject", false);
                if (HasPropertyObject) {
                        ::rmPropertyLinker::assingAsProperty(&Reserved1, "Reserved1", false);
                        ::rmPropertyLinker::assingAsProperty(&Reserved2, "Reserved2", false);
                        ::rmPropertyLinker::assingAsProperty(&ClassName, "ClassName", false);
                        ::rmPropertyLinker::assingAsProperty(&Reserved3, "Reserved3", false);
                        ::rmPropertyLinker::assingAsProperty(&Reserved4, "Reserved4", false);
                        ::rmPropertyLinker::assingAsProperty(&ClassVersion, "ClassVersion", false);
                        ::rmPropertyLinker::assingAsProperty(&Reserved5, "Reserved5", false);
                        ::rmPropertyLinker::assingAsProperty(&DataSize, "DataSize", false);


                        // Data.


                        // PropertyData.
                        ::rmPropertyLinker::assingAsProperty(&Version_, "Version", false);
                        ::rmPropertyLinker::assingAsProperty(&Count, "Count", false);
                        ::rmPropertyLinker::assingAsProperty(&Properties, "Properties");


                        ::rmPropertyLinker::assingAsProperty(&ClassData, "ClassData");
                }
        }


        bool eCMaterialResource2::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void eCMaterialResource2::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Version, "Version", false);
        }


        bool eCShaderEllementBase::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void eCShaderEllementBase::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Version, "Version", false);
                ::rmPropertyLinker::assingAsProperty(&Token, "Token");
                ::rmPropertyLinker::assingAsProperty(&EditorLayout, "EditorLayout");
                if (Version >= 2) {
                        ::rmPropertyLinker::assingAsProperty(&Unknown, "Unknown");
                }
        }


        bool eCShaderBase::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void eCShaderBase::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Version, "Version", false);
                ::rmPropertyLinker::assingAsProperty(&Inherited, "Inherited");
                ::rmPropertyLinker::assingAsProperty(&ShaderElementCount, "ShaderElementCount", false);
                ::rmPropertyLinker::assingAsProperty(&ShaderElements, "ShaderElements");
        }


        bool eCColorSrcProxy::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void eCColorSrcProxy::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Version, "Version", false);
                if (Version >= 666) {
                        ::rmPropertyLinker::assingAsProperty(&ColorComponent, "ColorComponent");
                        ::rmPropertyLinker::assingAsProperty(&ColorModifier, "ColorModifier");
                }
                ::rmPropertyLinker::assingAsProperty(&Token, "Token");
        }


        bool eCShaderDefault::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void eCShaderDefault::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Version, "Version", false);
                ::rmPropertyLinker::assingAsProperty(&ColorSrcDiffuse, "ColorSrcDiffuse");
                ::rmPropertyLinker::assingAsProperty(&ColorSrcOpacity, "ColorSrcOpacity");
                ::rmPropertyLinker::assingAsProperty(&ColorSrcSelfIllumination, "ColorSrcSelfIllumination");
                ::rmPropertyLinker::assingAsProperty(&ColorSrcSpecular, "ColorSrcSpecular");
                ::rmPropertyLinker::assingAsProperty(&ColorSrcSpecularPower, "ColorSrcSpecularPower");
                ::rmPropertyLinker::assingAsProperty(&ColorSrcNormal, "ColorSrcNormal");
                if (Version >= 2) {
                        ::rmPropertyLinker::assingAsProperty(&ColorSrcDistortion, "ColorSrcDistortion");
                }
                if (Version >= 3) {
                        ::rmPropertyLinker::assingAsProperty(&ColorSrcLightEmission, "ColorSrcLightEmission");
                }
                ::rmPropertyLinker::assingAsProperty(&Inherited, "Inherited");
        }


        bool eCColorScale::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void eCColorScale::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Version, "Version", false);
                ::rmPropertyLinker::assingAsProperty(&ArraySize, "ArraySize", false);
                ::rmPropertyLinker::assingAsProperty(&ItemCount, "ItemCount", false);
                ::rmPropertyLinker::assingAsProperty(&Items, "Items");
        }


        bool eCColorSrcBase::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void eCColorSrcBase::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Version, "Version", false);
                ::rmPropertyLinker::assingAsProperty(&Inherited, "Inherited");
        }


        bool eCColorSrcBlend::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void eCColorSrcBlend::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Version, "Version", false);
                ::rmPropertyLinker::assingAsProperty(&ColorSrc1, "ColorSrc1");
                ::rmPropertyLinker::assingAsProperty(&ColorSrc2, "ColorSrc2");
                ::rmPropertyLinker::assingAsProperty(&BlendSrc, "BlendSrc");
                ::rmPropertyLinker::assingAsProperty(&Inherited, "Inherited");
        }


        bool eCColorSrcCamDistance::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void eCColorSrcCamDistance::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Version, "Version", false);
                ::rmPropertyLinker::assingAsProperty(&Inherited, "Inherited");
        }


        bool eCColorSrcCamObstacle::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void eCColorSrcCamObstacle::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Version, "Version", false);
                ::rmPropertyLinker::assingAsProperty(&Inherited, "Inherited");
        }


        bool eCColorSrcCombiner::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void eCColorSrcCombiner::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Version, "Version", false);
                ::rmPropertyLinker::assingAsProperty(&ColorSrc1, "ColorSrc1");
                ::rmPropertyLinker::assingAsProperty(&ColorSrc2, "ColorSrc2");
                ::rmPropertyLinker::assingAsProperty(&Inherited, "Inherited");
        }


        bool eCColorSrcConstant::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void eCColorSrcConstant::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Version, "Version", false);
                ::rmPropertyLinker::assingAsProperty(&Inherited, "Inherited");
        }


        bool eCColorSrcConstantAnimation::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void eCColorSrcConstantAnimation::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Version, "Version", false);
                ::rmPropertyLinker::assingAsProperty(&ColorScale, "ColorScale");
                ::rmPropertyLinker::assingAsProperty(&Inherited, "Inherited");
        }


        bool eCColorSrcConstantSwitch::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void eCColorSrcConstantSwitch::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Version, "Version", false);
                ::rmPropertyLinker::assingAsProperty(&Inherited, "Inherited");
        }


        bool eCColorSrcCubeSampler::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void eCColorSrcCubeSampler::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Version, "Version", false);
                ::rmPropertyLinker::assingAsProperty(&Inherited, "Inherited");
        }


        bool eCColorSrcFresnel::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void eCColorSrcFresnel::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Version, "Version", false);
                ::rmPropertyLinker::assingAsProperty(&Inherited, "Inherited");
        }


        bool eCColorSrcMask::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void eCColorSrcMask::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Version, "Version", false);
                ::rmPropertyLinker::assingAsProperty(&Color, "Color");
                ::rmPropertyLinker::assingAsProperty(&Inherited, "Inherited");
        }


        bool eCColorSrcSkydomeSampler::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void eCColorSrcSkydomeSampler::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Version, "Version", false);
                ::rmPropertyLinker::assingAsProperty(&Inherited, "Inherited");
        }


        bool eCColorSrcVertexColor::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void eCColorSrcVertexColor::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Version, "Version", false);
                ::rmPropertyLinker::assingAsProperty(&Inherited, "Inherited");
        }


        bool eCTexCoordSrcProxy::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void eCTexCoordSrcProxy::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&VertexTexCoord, "VertexTexCoord");
                ::rmPropertyLinker::assingAsProperty(&Token, "Token");
        }


        bool eCColorSrcSampler::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void eCColorSrcSampler::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Version, "Version", false);
                ::rmPropertyLinker::assingAsProperty(&TexCoord, "TexCoord");
                ::rmPropertyLinker::assingAsProperty(&Unknown, "Unknown");
                ::rmPropertyLinker::assingAsProperty(&Inherited, "Inherited");
        }


        bool eCTexCoordSrcBase::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void eCTexCoordSrcBase::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Version, "Version", false);
                ::rmPropertyLinker::assingAsProperty(&Inherited, "Inherited");
        }


        bool eCTexCoordSrcBumpOffset::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void eCTexCoordSrcBumpOffset::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Version, "Version", false);
                ::rmPropertyLinker::assingAsProperty(&Height, "Height");
                ::rmPropertyLinker::assingAsProperty(&TexCoord, "TexCoord");
                ::rmPropertyLinker::assingAsProperty(&Inherited, "Inherited");
        }


        bool eCTexCoordSrcColor::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void eCTexCoordSrcColor::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Version, "Version", false);
                ::rmPropertyLinker::assingAsProperty(&ColorU, "ColorU");
                ::rmPropertyLinker::assingAsProperty(&ColorV, "ColorV");
                ::rmPropertyLinker::assingAsProperty(&Inherited, "Inherited");
        }


        bool eCTexCoordSrcOscillator::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void eCTexCoordSrcOscillator::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&TexCoord, "TexCoord");
                ::rmPropertyLinker::assingAsProperty(&Inherited, "Inherited");
        }


        bool eCTexCoordSrcReflect::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void eCTexCoordSrcReflect::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Inherited, "Inherited");
        }


        bool eCTexCoordSrcRotator::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void eCTexCoordSrcRotator::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Version, "Version", false);
                ::rmPropertyLinker::assingAsProperty(&TexCoord, "TexCoord");
                ::rmPropertyLinker::assingAsProperty(&Inherited, "Inherited");
        }


        bool eCTexCoordSrcScale::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void eCTexCoordSrcScale::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Version, "Version", false);
                ::rmPropertyLinker::assingAsProperty(&TexCoord, "TexCoord");
                ::rmPropertyLinker::assingAsProperty(&Inherited, "Inherited");
        }


        bool eCTexCoordSrcScroller::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void eCTexCoordSrcScroller::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Version, "Version", false);
                ::rmPropertyLinker::assingAsProperty(&TexCoord, "TexCoord");
                ::rmPropertyLinker::assingAsProperty(&Inherited, "Inherited");
        }


        bool eCShaderLeafs::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void eCShaderLeafs::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&eCShaderDefault, "eCShaderDefault");
        }


        bool eCShaderLightStreaks::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void eCShaderLightStreaks::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Version, "Version", false);
                ::rmPropertyLinker::assingAsProperty(&ColorSrcDiffuse, "ColorSrcDiffuse");
                ::rmPropertyLinker::assingAsProperty(&Inherited, "Inherited");
        }


        bool eCShaderParticle::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void eCShaderParticle::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Version, "Version", false);
                ::rmPropertyLinker::assingAsProperty(&ColorSrcDiffuse, "ColorSrcDiffuse");
                ::rmPropertyLinker::assingAsProperty(&Inherited, "Inherited");
        }


        bool eCShaderSkyDome::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void eCShaderSkyDome::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Version, "Version", false);
                ::rmPropertyLinker::assingAsProperty(&ColorSrcClouds, "ColorSrcClouds");
                ::rmPropertyLinker::assingAsProperty(&ColorSrcAbsorbation, "ColorSrcAbsorbation");
                ::rmPropertyLinker::assingAsProperty(&Inherited, "Inherited");
        }


        bool xmatFile::doSpecialPropAssign(string name)
        {
                WrittenClassBase::doSpecialPropAssign("");
                return false;
        }


        void xmatFile::assignVarsAsProps(void)
        {
                WrittenClassBase::assignVarsAsProps();
                ::rmPropertyLinker::assingAsProperty(&Revision, "Revision", false);
                ::rmPropertyLinker::assingAsProperty(&DataRevision, "DataRevision", false);
                ::rmPropertyLinker::assingAsProperty(&PropertyObjectOffset, "PropertyObjectOffset", false);
                ::rmPropertyLinker::assingAsProperty(&PropertyObjectSize, "PropertyObjectSize", false);
                ::rmPropertyLinker::assingAsProperty(&ResourceDataOffset, "ResourceDataOffset", false);
                ::rmPropertyLinker::assingAsProperty(&ResourceDataSize, "ResourceDataSize", false);
                ::rmPropertyLinker::assingAsProperty(&RawModificationTime, "RawModificationTime", false);
                ::rmPropertyLinker::assingAsProperty(&RawFileExtension, "RawFileExtension[8]", false);
                ::rmPropertyLinker::assingAsProperty(&Header, "Header");
                ::rmPropertyLinker::assingAsProperty(&Data, "Data");
        }
};

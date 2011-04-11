#pragma once

//#include "rmPropertyLinker.h"
#include "rmFileStream.h"

#define BCSTRING_SIMPLE
#define BCGUID_SIMPLE

namespace rmDummyClass
{
	class WrittenClassBase
	{
	public:
		WrittenClassBase(void);
		WrittenClassBase(const WrittenClassBase & src);
		virtual ~WrittenClassBase(void);
		virtual WrittenClassBase * clone(void) const;

		virtual void read(rmInFileStream & fs);
		virtual void write(rmOutFileStream & fs) const;
		virtual void asciiWrite(rmOutFileStream & fs, string indent) const;
		virtual rmU32 getWriteSize(void) const;
		virtual void assignVarsAsProps(void);
                virtual bool doSpecialPropAssign(string name);

		string getClassName(void) const;
		rmBool isSimpleClass(void) const;

	protected:
		void setClassName(string name);

		static rmOut out;

		rmBool simple;

	private:
		string className;
                public:rmBool isInitialized;//$
	};

	class DummyClassManager
	{
	private:
		enum EDummyClass
		{
			EbCAccessorProperty,
			EbCAccessorPropertyObject,
			EbCEulerAngles,
			EbCFloatColor,
			EbCGuid,
			EbCImageOrMaterialResourceString,
			EbCImageResourceString,
			EbCRect,
			EbCString,
			EbCVector2,
			EGEBool,
			EGEChar,
			EeCColorScale,
			EeCColorSrcBase,
			EeCColorSrcBlend,
			EeCColorSrcCamDistance,
			EeCColorSrcCamObstacle,
			EeCColorSrcCombiner,
			EeCColorSrcConstant,
			EeCColorSrcConstantAnimation,
			EeCColorSrcConstantSwitch,
			EeCColorSrcCubeSampler,
			EeCColorSrcFresnel,
			EeCColorSrcMask,
			EeCColorSrcProxy,
			EeCColorSrcSampler,
			EeCColorSrcSkydomeSampler,
			EeCColorSrcVertexColor,
			EeCMaterialResource2,
			EeCShaderBase,
			EeCShaderDefault,
			EeCShaderEllementBase,
			EeCShaderLeafs,
			EeCShaderLightStreaks,
			EeCShaderParticle,
			EeCShaderSkyDome,
			EeCTexCoordSrcBase,
			EeCTexCoordSrcBumpOffset,
			EeCTexCoordSrcColor,
			EeCTexCoordSrcOscillator,
			EeCTexCoordSrcProxy,
			EeCTexCoordSrcReflect,
			EeCTexCoordSrcRotator,
			EeCTexCoordSrcScale,
			EeCTexCoordSrcScroller,
			EGEFloat,
			EGEInt,
			EDummyClassCount
		};
		enum EDummyTemplateClass
		{
			EbTPropertyContainer,
			EDummyTemplateClassCount
		};
	
	public:
		static bool assignDummyClass(WrittenClassBase *& target, string name, rmU32 size = (-1));

		static void asciiWriteAsMember(rmOutFileStream & fs, string indent, const WrittenClassBase * data, rmLPCChar memberName, rmLPCChar typeName = "");

                enum {NEnums = 16, MaxEnumValueCount = 19};
		static const rmLPCChar enumValueStrings[][MaxEnumValueCount];
		static const rmLPCChar enumNames[NEnums];
		static const rmU32 enumStringListSizes[NEnums];
	private:
		static const rmLPCChar DummyClassNames[EDummyClassCount];
		static const rmLPCChar DummyTemplateClassNames[EDummyTemplateClassCount];

		static rmOut out;
	};

        class HexData : public WrittenClassBase
        {
        public:
                HexData(rmU32 size, string name);
                                HexData(const HexData & src);
                ~HexData(void);
                                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                vector<rmU8> Data;
        };

        class EnumValue : public WrittenClassBase
        {
        public:
                EnumValue(const string & typeName);
                EnumValue(const EnumValue & src);
                ~EnumValue(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

                rmU32 & getValue(void);

        protected:
                rmU32 Value;

                friend class eCColorSrcProxy;
        };

        class bCGuid : public WrittenClassBase
        {
        public:
                bCGuid(void);
                bCGuid(const bCGuid & src);
                ~bCGuid(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmGuid Guid; // GUID.
                rmBool Valid; // GEBool.
                list<rmU8> Alignment; // GEU8.
        };

        class GEBool : public WrittenClassBase
        {
        public:
                GEBool(void);
                GEBool(const GEBool & src);
                ~GEBool(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmBool Value;
        };

        class GEChar : public WrittenClassBase
        {
        public:
                GEChar(void);
                GEChar(const GEChar & src);
                ~GEChar(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmChar Value;
        };

        class GEInt : public WrittenClassBase
        {
        public:
                GEInt(void);
                GEInt(const GEInt & src);
                ~GEInt(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmI32 Value;
        };

        class GEFloat : public WrittenClassBase
        {
        public:
                GEFloat(void);
                GEFloat(const GEFloat & src);
                ~GEFloat(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmF32 Value;
        };

        class bCString : public WrittenClassBase
        {
        public:
                bCString(void);
                bCString(const bCString & src);
                bCString(string typeName);
                ~bCString(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                string Value;

                friend class bCAccessorProperty;
                friend class bCAccessorPropertyObject;
        };

        class bTPropertyContainer : public WrittenClassBase
        {
        public:
                bTPropertyContainer(string templateArgument);
                bTPropertyContainer(const bTPropertyContainer & src);
                ~bTPropertyContainer(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmU16 Version;
                WrittenClassBase * Value;
        };

        class bCAccessorProperty : public WrittenClassBase
        {
        public:
                bCAccessorProperty(void);
                bCAccessorProperty(const bCAccessorProperty & src);
                ~bCAccessorProperty(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                bCString Name;
                bCString Type;
                rmU16 Version;
                WrittenClassBase * Data;
        };

        class bCFloatColor : public WrittenClassBase
        {
        public:
                bCFloatColor();
                bCFloatColor(const bCFloatColor & src);
                ~bCFloatColor(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmU32 __vftbl;
                rmF32 R;
                rmF32 G;
                rmF32 B;
        };

        class bCEulerAngles : public WrittenClassBase
        {
        public:
                bCEulerAngles();
                bCEulerAngles(const bCEulerAngles & src);
                ~bCEulerAngles(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmF32 Pitch;
                rmF32 Roll;
                rmF32 Yaw;
        };

        class bCVector2 : public WrittenClassBase
        {
        public:
                bCVector2();
                bCVector2(const bCVector2 & src);
                ~bCVector2(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmF32 X;
                rmF32 Y;
        };

        class bCRect : public WrittenClassBase
        {
        public:
                bCRect();
                bCRect(const bCRect & src);
                ~bCRect(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmU32 Unknown1;
                rmU32 Unknown2;
                rmU32 Unknown3;
                rmU32 Unknown4;
        };

        class bCAccessorPropertyObject : public WrittenClassBase
        {
        public:
                bCAccessorPropertyObject();
                bCAccessorPropertyObject(const bCAccessorPropertyObject & src);
                ~bCAccessorPropertyObject(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmU16 Version;
                rmBool HasPropertyObject;

                // if (HasPropertyObject).

                rmU16 Reserved1;
                rmBool Reserved2;
                bCString ClassName;
                rmU16 Reserved3;
                rmBool Reserved4;
                rmU16 ClassVersion;
                rmU16 Reserved5;
                rmU32 DataSize;

                // Data.

                // PropertyData.
                rmU16 Version_;
                rmU32 Count;
                list<bCAccessorProperty> Properties;

                // ClassData.
                WrittenClassBase * ClassData;
        };

        class eCMaterialResource2 : public WrittenClassBase
        {
        public:
                eCMaterialResource2();
                eCMaterialResource2(const eCMaterialResource2 & src);
                ~eCMaterialResource2(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmU16 Version;
        };

        class eCShaderEllementBase : public WrittenClassBase
        {
        public:
                eCShaderEllementBase();
                eCShaderEllementBase(const eCShaderEllementBase & src);
                ~eCShaderEllementBase(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmU16 Version;
                bCGuid Token;
                bCRect EditorLayout;
                rmBool Unknown; // If Version >= 2.
        };

        class eCShaderBase : public WrittenClassBase
        {
        public:
                eCShaderBase();
                eCShaderBase(const eCShaderBase & src);
                ~eCShaderBase(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmU16 Version;
                eCShaderEllementBase Inherited;
                rmU32 ShaderElementCount;
                list<bCAccessorPropertyObject *> ShaderElements; // bCAccessorPropertyObject[ShaderElementCount].
        };

        class eCColorSrcProxy : public WrittenClassBase
        {
        public:
                eCColorSrcProxy(void);
                eCColorSrcProxy(const eCColorSrcProxy & src);
                ~eCColorSrcProxy(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmU32 Version; // GEU32 (eEShaderColorSrcComponent ??).
                EnumValue * ColorComponent; // eEShaderColorSrcComponent; If Version >= 666.
                EnumValue * ColorModifier; // eEShaderColorModifier; If Version >= 666.
                bCGuid Token;
        };

        class eCShaderDefault : public WrittenClassBase
        {
        public:
                eCShaderDefault();
                eCShaderDefault(const eCShaderDefault & src);
                ~eCShaderDefault(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmU16 Version;
                eCColorSrcProxy ColorSrcDiffuse;
                eCColorSrcProxy ColorSrcOpacity;
                eCColorSrcProxy ColorSrcSelfIllumination;
                eCColorSrcProxy ColorSrcSpecular;
                eCColorSrcProxy ColorSrcSpecularPower;
                eCColorSrcProxy ColorSrcNormal;
                eCColorSrcProxy ColorSrcDistortion; // If Version >= 2.
                eCColorSrcProxy ColorSrcLightEmission; // If Version >= 3.
                eCShaderBase Inherited;
        };

        class eCColorScale : public WrittenClassBase
        {
        public:
                struct ColorScaleItem
                {
                        rmF32 Time;
                        rmF32 Red;
                        rmF32 Green;
                        rmF32 Blue;
                        rmF32 Alpha;
                };

                eCColorScale(void);
                eCColorScale(const eCColorScale & src);
                ~eCColorScale(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmU32 Version;
                rmU32 ArraySize;
                rmU32 ItemCount;
                list<ColorScaleItem> Items;
        };

        class eCColorSrcBase : public WrittenClassBase
        {
        public:
                eCColorSrcBase(void);
                eCColorSrcBase(const eCColorSrcBase & src);
                ~eCColorSrcBase(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmU16 Version;
                eCShaderEllementBase Inherited;
        };

        class eCColorSrcBlend : public WrittenClassBase
        {
        public:
                eCColorSrcBlend(void);
                eCColorSrcBlend(const eCColorSrcBlend & src);
                ~eCColorSrcBlend(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmU16 Version;
                eCColorSrcProxy ColorSrc1;
                eCColorSrcProxy ColorSrc2;
                eCColorSrcProxy BlendSrc;
                eCColorSrcBase Inherited;
        };

        class eCColorSrcCamDistance : public WrittenClassBase
        {
        public:
                eCColorSrcCamDistance(void);
                eCColorSrcCamDistance(const eCColorSrcCamDistance & src);
                ~eCColorSrcCamDistance(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmU16 Version;
                eCColorSrcBase Inherited;
        };

        class eCColorSrcCamObstacle : public WrittenClassBase
        {
        public:
                eCColorSrcCamObstacle(void);
                eCColorSrcCamObstacle(const eCColorSrcCamObstacle & src);
                ~eCColorSrcCamObstacle(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmU16 Version;
                eCColorSrcBase Inherited;
        };

        class eCColorSrcCombiner : public WrittenClassBase
        {
        public:
                eCColorSrcCombiner(void);
                eCColorSrcCombiner(const eCColorSrcCombiner & src);
                ~eCColorSrcCombiner(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmU16 Version;
                eCColorSrcProxy ColorSrc1;
                eCColorSrcProxy ColorSrc2;
                eCColorSrcBase Inherited;
        };

        class eCColorSrcConstant : public WrittenClassBase
        {
        public:
                eCColorSrcConstant(void);
                eCColorSrcConstant(const eCColorSrcConstant & src);
                ~eCColorSrcConstant(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmU16 Version;
                eCColorSrcBase Inherited;
        };

        class eCColorSrcConstantAnimation : public WrittenClassBase
        {
        public:
                eCColorSrcConstantAnimation(void);
                eCColorSrcConstantAnimation(const eCColorSrcConstantAnimation & src);
                ~eCColorSrcConstantAnimation(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmU16 Version;
                eCColorScale ColorScale;
                eCColorSrcBase Inherited;
        };

        class eCColorSrcConstantSwitch : public WrittenClassBase
        {
        public:
                eCColorSrcConstantSwitch(void);
                eCColorSrcConstantSwitch(const eCColorSrcConstantSwitch & src);
                ~eCColorSrcConstantSwitch(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmU16 Version;
                eCColorSrcBase Inherited;
        };

        class eCColorSrcCubeSampler : public WrittenClassBase
        {
        public:
                eCColorSrcCubeSampler(void);
                eCColorSrcCubeSampler(const eCColorSrcCubeSampler & src);
                ~eCColorSrcCubeSampler(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmU16 Version;
                eCColorSrcBase Inherited;
        };

        class eCColorSrcFresnel : public WrittenClassBase
        {
        public:
                eCColorSrcFresnel(void);
                eCColorSrcFresnel(const eCColorSrcFresnel & src);
                ~eCColorSrcFresnel(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmU16 Version;
                eCColorSrcBase Inherited;
        };

        class eCColorSrcMask : public WrittenClassBase
        {
        public:
                eCColorSrcMask(void);
                eCColorSrcMask(const eCColorSrcMask & src);
                ~eCColorSrcMask(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmU16 Version;
                eCColorSrcProxy Color;
                eCColorSrcBase Inherited;
        };

        class eCColorSrcSkydomeSampler : public WrittenClassBase
        {
        public:
                eCColorSrcSkydomeSampler(void);
                eCColorSrcSkydomeSampler(const eCColorSrcSkydomeSampler & src);
                ~eCColorSrcSkydomeSampler(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmU16 Version;
                eCColorSrcBase Inherited;
        };

        class eCColorSrcVertexColor : public WrittenClassBase
        {
        public:
                eCColorSrcVertexColor(void);
                eCColorSrcVertexColor(const eCColorSrcVertexColor & src);
                ~eCColorSrcVertexColor(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmU16 Version;
                eCColorSrcBase Inherited;
        };

        class eCTexCoordSrcProxy : public WrittenClassBase
        {
        public:
                eCTexCoordSrcProxy();
                eCTexCoordSrcProxy(const eCTexCoordSrcProxy & src);
                ~eCTexCoordSrcProxy(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmU32 VertexTexCoord;
                bCGuid Token;
        };

        class eCColorSrcSampler : public WrittenClassBase
        {
        public:
                eCColorSrcSampler(void);
                eCColorSrcSampler(const eCColorSrcSampler & src);
                ~eCColorSrcSampler(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmU16 Version;
                eCTexCoordSrcProxy TexCoord;
                rmU32 Unknown;
                eCColorSrcBase Inherited;
        };

        class eCTexCoordSrcBase : public WrittenClassBase
        {
        public:
                eCTexCoordSrcBase();
                eCTexCoordSrcBase(const eCTexCoordSrcBase & src);
                ~eCTexCoordSrcBase(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmU16 Version;
                eCShaderEllementBase Inherited;
        };

        class eCTexCoordSrcBumpOffset : public WrittenClassBase
        {
        public:
                eCTexCoordSrcBumpOffset();
                eCTexCoordSrcBumpOffset(const eCTexCoordSrcBumpOffset & src);
                ~eCTexCoordSrcBumpOffset(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmU16 Version;
                eCColorSrcProxy Height;
                eCTexCoordSrcProxy TexCoord;
                eCTexCoordSrcBase Inherited;
        };

        class eCTexCoordSrcColor : public WrittenClassBase
        {
        public:
                eCTexCoordSrcColor();
                eCTexCoordSrcColor(const eCTexCoordSrcColor & src);
                ~eCTexCoordSrcColor(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmU16 Version;
                eCColorSrcProxy ColorU;
                eCColorSrcProxy ColorV;
                eCTexCoordSrcBase Inherited;
        };

        class eCTexCoordSrcOscillator : public WrittenClassBase
        {
        public:
                eCTexCoordSrcOscillator();
                eCTexCoordSrcOscillator(const eCTexCoordSrcOscillator & src);
                ~eCTexCoordSrcOscillator(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                eCTexCoordSrcProxy TexCoord;
                eCTexCoordSrcBase Inherited;
        };

        class eCTexCoordSrcReflect : public WrittenClassBase
        {
        public:
                eCTexCoordSrcReflect();
                eCTexCoordSrcReflect(const eCTexCoordSrcReflect & src);
                ~eCTexCoordSrcReflect(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                eCTexCoordSrcBase Inherited;
        };

        class eCTexCoordSrcRotator : public WrittenClassBase
        {
        public:
                eCTexCoordSrcRotator();
                eCTexCoordSrcRotator(const eCTexCoordSrcRotator & src);
                ~eCTexCoordSrcRotator(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmU16 Version;
                eCTexCoordSrcProxy TexCoord;
                eCTexCoordSrcBase Inherited;
        };

        class eCTexCoordSrcScale : public WrittenClassBase
        {
        public:
                eCTexCoordSrcScale();
                eCTexCoordSrcScale(const eCTexCoordSrcScale & src);
                ~eCTexCoordSrcScale(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmU16 Version;
                eCTexCoordSrcProxy TexCoord;
                eCTexCoordSrcBase Inherited;
        };

        class eCTexCoordSrcScroller : public WrittenClassBase
        {
        public:
                eCTexCoordSrcScroller();
                eCTexCoordSrcScroller(const eCTexCoordSrcScroller & src);
                ~eCTexCoordSrcScroller(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmU16 Version;
                eCTexCoordSrcProxy TexCoord;
                eCTexCoordSrcBase Inherited;
        };

        class eCShaderLeafs : public WrittenClassBase
        {
        public:
                eCShaderLeafs();
                eCShaderLeafs(const eCShaderLeafs & src);
                ~eCShaderLeafs(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                eCShaderDefault eCShaderDefault;
        };

        class eCShaderLightStreaks : public WrittenClassBase
        {
        public:
                eCShaderLightStreaks();
                eCShaderLightStreaks(const eCShaderLightStreaks & src);
                ~eCShaderLightStreaks(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmU16 Version;
                eCColorSrcProxy ColorSrcDiffuse;
                eCShaderBase Inherited;
        };

        class eCShaderParticle : public WrittenClassBase
        {
        public:
                eCShaderParticle();
                eCShaderParticle(const eCShaderParticle & src);
                ~eCShaderParticle(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmU16 Version;
                eCColorSrcProxy ColorSrcDiffuse;
                eCShaderBase Inherited;
        };

        class eCShaderSkyDome : public WrittenClassBase
        {
        public:
                eCShaderSkyDome();
                eCShaderSkyDome(const eCShaderSkyDome & src);
                ~eCShaderSkyDome(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                rmU16 Version;
                eCColorSrcProxy ColorSrcClouds;
                eCColorSrcProxy ColorSrcAbsorbation;
                eCShaderBase Inherited;
        };

        class xmatFile : public WrittenClassBase
        {
        public:
                xmatFile();
                xmatFile(const xmatFile & src);
                ~xmatFile(void);
                WrittenClassBase * clone(void) const;
                void read(rmInFileStream & fs);
                void write(rmOutFileStream & fs) const;
                void asciiWrite(rmOutFileStream & fs, string indent) const;
                rmU32 getWriteSize(void) const;
                void assignVarsAsProps(void);
                bool doSpecialPropAssign(string name);

        protected:
                string Revision; // GEU32.
                string DataRevision; // GEU32.
                rmU32 PropertyObjectOffset;
                rmU32 PropertyObjectSize;
                rmU32 ResourceDataOffset;
                rmU32 ResourceDataSize;
                rmU64 RawModificationTime;
                string RawFileExtension;
                bCAccessorPropertyObject Header;
                bCAccessorPropertyObject Data;
        };
}

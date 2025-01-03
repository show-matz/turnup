//------------------------------------------------------------------------------
//
// InputFile.hxx
//
//------------------------------------------------------------------------------
#ifndef INPUTFILE_HXX__
#define INPUTFILE_HXX__

#include <stdint.h>

namespace turnup {

    class TextSpan;

    //--------------------------------------------------------------------------
    //
    // class InputFile
    //
    //--------------------------------------------------------------------------
    class InputFile {
    public:
        InputFile();
        virtual ~InputFile();
    public:
        virtual const TextSpan& GetFileName() const = 0;
        virtual uint32_t LineSize() const = 0;
        virtual const TextSpan* LineTop() const = 0;
        virtual const TextSpan* LineEnd() const = 0;
    public:
        static InputFile* LoadInputFile( const TextSpan& filePath );
        static void ReleaseInputFile( InputFile* pInputFile );
    };


} // namespace turnup

#endif // INPUTFILE_HXX__

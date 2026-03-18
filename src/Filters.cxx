//------------------------------------------------------------------------------
//
// Filters.hxx
//
//------------------------------------------------------------------------------
#include "Filters.hxx"

#include "DocumentInfo.hxx"
#include "StyleStack.hxx"
#include "TextSpan.hxx"
#include "TextMaker.hxx"
#include "InternalFilter.hxx"
#include "InternalFilter4Default.hxx"

//-------------------
#include "CRC64.hxx"
#include "File.hxx"
#include <stdlib.h>
#include <fstream>
//-------------------

#include <iostream>
#include <utility>
#include <vector>
#include <algorithm>
#include <string.h>

namespace turnup {

    static uint64_t CalcRangeCRC( const TextSpan* pTop,
                                  const TextSpan* pEnd, char* pTagBuf );
    static bool ExecExtFilter( std::ostream& os,
                               const TextSpan& type, const TextSpan& cmd,
                               const TextSpan* pTop, const TextSpan* pEnd );

    //--------------------------------------------------------------------------
    //
    // class Filters::Impl
    //
    //--------------------------------------------------------------------------
    class Filters::Impl {
    private:
        typedef std::pair<TextSpan,TextSpan>    ExtFilter;
        typedef std::vector<ExtFilter>          ExtFilterList;
    public:
        Impl();
        ~Impl();
    public:
        void RegistExternal( const TextSpan& label, const TextSpan& command );
        void RegistDefault( const TextSpan& command );
        bool ExecuteFilter( std::ostream& os, 
                            DocumentInfo& docInfo,
                            const TextSpan& type,
                            const TextSpan* pTop, const TextSpan* pEnd );
    private:
        ExtFilterList m_externals;
        TextSpan      m_defaultFilter;
    };


    //--------------------------------------------------------------------------
    //
    // implementation of class Filters
    //
    //--------------------------------------------------------------------------
    Filters::Filters() : m_pImpl( new Impl{} ) {
    }
    Filters::~Filters() {
        delete m_pImpl;
    }
    void Filters::RegistExternal( const TextSpan& label, const TextSpan& command ) {
        return m_pImpl->RegistExternal( label, command );
    }
    void Filters::RegistDefault( const TextSpan& command ) {
        return m_pImpl->RegistDefault( command );
    }
    bool Filters::ExecuteFilter( std::ostream& os,
                                 DocumentInfo& docInfo,
                                 const TextSpan& type,
                                 const TextSpan* pTop, const TextSpan* pEnd ) {
        return m_pImpl->ExecuteFilter( os, docInfo, type, pTop, pEnd );
    }

    //--------------------------------------------------------------------------
    //
    // implementation of class Filters::Impl
    //
    //--------------------------------------------------------------------------
    Filters::Impl::Impl() : m_externals( {} ),
                            m_defaultFilter() {
    }
    Filters::Impl::~Impl() {
        m_externals.clear();
    }
    void Filters::Impl::RegistDefault( const TextSpan& command ) {
        m_defaultFilter = command;
    }
    void Filters::Impl::RegistExternal( const TextSpan& label, const TextSpan& command ) {
        m_externals.emplace_back( label, command );
    }
    bool Filters::Impl::ExecuteFilter( std::ostream& os,
                                       DocumentInfo& docInfo,
                                       const TextSpan& type,
                                       const TextSpan* pTop, const TextSpan* pEnd ) {
        // type 指定がなければデフォルトの <pre> 出力で終了
        if( type.IsEmpty() ) {
            InternalFilter4Default( os, docInfo, pTop, pEnd );
            return true;
        }
        /* 外部フィルタを優先して検索 */ {
            auto itr = std::find_if( m_externals.begin(), m_externals.end(),
                                     [&type]( const ExtFilter& filter ) -> bool {
                                         return filter.first.IsEqual( type );
                                     } );
            // 該当する外部フィルタが見つかれば実行して終了
            if( itr != m_externals.end() )
                return ExecExtFilter( os, type, itr->second, pTop, pEnd );
        }
        /* 該当する外部フィルタがなければ次に内部フィルタを検索 */ {
            auto pFilter = InternalFilter::FindFilter( type );
            if( pFilter ) {
                // 該当する内部フィルタが見つかれば実行して終了
                return pFilter( os, docInfo, pTop, pEnd );
            }
        }
        // デフォルトフィルタの指定があればそれを実行
        if( m_defaultFilter.IsEmpty() == false )
            return ExecExtFilter( os, type, m_defaultFilter, pTop, pEnd );

        // 上記以外の場合、デフォルトの <pre> 出力で false 復帰
        InternalFilter4Default( os, docInfo, pTop, pEnd );
        return false;
    }

    //--------------------------------------------------------------------------
    //
    // local functions
    //
    //--------------------------------------------------------------------------
    static uint64_t CalcRangeCRC( const TextSpan* pTop,
                                  const TextSpan* pEnd, char* pTagBuf ) {
        // [ pTop->Top(), pEnd->Top() ) が include でつぎはぎされた領域の場合、メモリ上で
        // 連続している保証がない。そのため、ある程度頑張ってあげる必要がある。
        struct Data {
            const TextSpan* pTS1;
            const TextSpan* pTS2;
            const char*     pCur;
        } data{ pTop, pEnd, pTop->Top() };
        auto callback = []( void* pOpaque ) -> const uint8_t* {
            Data* p = reinterpret_cast<Data*>( pOpaque );
            if( p->pTS1 == p->pTS2 )
                return nullptr;
            while( true ) {
                if( p->pCur < p->pTS1->End() )
                    break;
                p->pTS1 += 1;
                if( p->pTS1 == p->pTS2 )
                    return nullptr;
                p->pCur = p->pTS1->Top();
            }
            auto pRet = reinterpret_cast<const uint8_t*>( p->pCur );
            p->pCur += 1;
            return pRet;
        };
        return CRC64::Calc( 'X', callback, &data, pTagBuf ); // X means 'something other else'.
    }    
    static bool ExecExtFilter( std::ostream& os,
                               const TextSpan& type, const TextSpan& cmd,
                               const TextSpan* pTop, const TextSpan* pEnd ) {
        char inFile[16];
        char outFile[16];
        CalcRangeCRC( pTop, pEnd, inFile );
        ::strcpy( outFile, inFile );
        ::strcat( inFile,  ".in" );
        ::strcat( outFile, ".out" );
        std::ofstream ofs{ inFile };
        for( ; pTop < pEnd; ++pTop ) {
            ofs.write( pTop->Top(), pTop->End() - pTop->Top() );
            ofs << std::endl;
        }
        const char* pCmdTop = cmd.Top();
        const char* pCmdEnd = cmd.End();

        TextMaker tm{};
        while( pCmdTop < pCmdEnd ) {
            const char* p = std::find( pCmdTop, pCmdEnd, '%' );
            if( p == pCmdEnd ) {
                tm << TextSpan{ pCmdTop, pCmdEnd };
                pCmdTop = pCmdEnd;
            } else {
                if( pCmdTop < p )
                    tm << TextSpan{ pCmdTop, p };
                if( !::strncmp( p, "%in", 3 ) ) {
                    tm << inFile;
                    pCmdTop = p + 3;
                } else if( !::strncmp( p, "%out", 4 ) ) {
                    tm << outFile;
                    pCmdTop = p + 4;
                } else if( !::strncmp( p, "%type", 5 ) ) {
                    tm << type;
                    pCmdTop = p + 5;
                } else {
                    tm << TextSpan{ p, p+1 };
                    pCmdTop += 1;
                }
            }
        }
        ::system( tm.GetSpan().Top() );

        WholeFile* pSVG = File::LoadWhole( outFile );
        os << pSVG->GetBuffer<char>() << std::endl;
        File::ReleaseWholeFile( pSVG );

        // remove temporary file
        File::Remove( inFile );
        File::Remove( outFile );

        return true;
    }

} // namespace turnup


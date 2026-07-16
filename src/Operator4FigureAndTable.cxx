//------------------------------------------------------------------------------
//
// Operator4FigureAndTable.cxx
//
//------------------------------------------------------------------------------
#include "Operator4FigureAndTable.hxx"

#include "DocumentInfo.hxx"
#include "StyleStack.hxx"
#include "TextSpan.hxx"
#include "Config.hxx"
#include "ToC.hxx"

#include <stdint.h>
#include <iostream>

namespace turnup {

    static void WriteTitle( ToC::EntryT type, const TextSpan& title, DocumentInfo& docInfo );

    static const char* const s_classes[]  = { "", " class='tbl_title'", " class='fig_title'" };
    static ToC::EntryT       s_lastType   = ToC::EntryT::HEADER;
    static TextSpan          s_lastTitle{};
    static bool              s_titleTop[] = { false, true, false };    // 将来設定で変更可能にする想定

    const TextSpan* Operator4FigureAndTable1( const TextSpan* pTop,
                                              const TextSpan* pEnd, DocumentInfo& docInfo ) {
        (void)pEnd;

        ToC::EntryT type;
        TextSpan    title;
        if( pTop->IsMatch( "Figure.", title, "" ) )
            type = ToC::EntryT::FIGURE;
        else if ( pTop->IsMatch( "Table.", title, "" ) )
            type = ToC::EntryT::TABLE;
        else
            return pTop;

        title = title.Trim();

        auto& toc    = docInfo.Get<ToC>();
        auto& styles = docInfo.Get<StyleStack>();
        bool  pDuplicated = false;
        const char* pTag = toc.GetAnchorTag( type, pDuplicated, title.Top(), title.End() );
        //Duplicated な図表タイトルでも、ここではリンク先を生成するだけなのでエラーにはしない

        styles.WriteOpenTag( std::cout, "p", s_classes[(uint32_t)type] );
        if( pTag )
            std::cout << "<a name='" << pTag << "'></a>";
        else {
            //ToDo : error message...
        }

        auto& cfg = docInfo.Get<Config>(); {
            char chapter[64];
            toc.GetEntryNumber( chapter, type, cfg, title.Top(), title.End() );
            std::cout << chapter << ' ';
        }

        title.WriteTo( std::cout, docInfo,
                       cfg.bTermLinkInHeader ) << "</p>" << std::endl;
        return pTop + 1;
    }


    const TextSpan* Operator4FigureAndTable2( const TextSpan* pTop,
                                              const TextSpan* pEnd, DocumentInfo& docInfo ) {
        (void)pEnd;
        TextSpan param1;
        TextSpan param2;
        if( pTop->TrimTail().IsMatch( "<!-- ", param1, ":",
                                               param2, " -->" ) == false )
            return pTop;

        param1 = param1.Trim();
        param2 = param2.Trim();
        ToC::EntryT type;
        if( param1.IsEqual( "figure" ) )
            type = ToC::EntryT::FIGURE;
        else if( param1.IsEqual( "table" ) )
            type = ToC::EntryT::TABLE;
        else
            return pTop;

        if( param2.IsEqual( "end" ) ) {
            if( s_lastType == type ) {
                std::cout << "</a>" << std::endl;
                if( s_titleTop[(uint32_t)type] == false && s_lastTitle.IsEmpty() == false ) {
                    WriteTitle( type, s_lastTitle, docInfo );
                    s_lastType  = ToC::EntryT::HEADER;
                    s_lastTitle = TextSpan{};
                }
            }
        } else {
            if( s_titleTop[(uint32_t)type] == true ) 
                WriteTitle( type, param2, docInfo );
            else {
                s_lastType  = type;
                s_lastTitle = param2;
            }
            auto& toc         = docInfo.Get<ToC>();
            bool  pDuplicated = false;
            const char* pTag = toc.GetAnchorTag( type, pDuplicated, param2.Top(), param2.End() );
            //Duplicated な図表タイトルでも、ここではリンク先を生成するだけなのでエラーにはしない
            std::cout << "<a name='" << pTag << "'>" << std::endl;
        }
        return pTop + 1;
    }



    static void WriteTitle( ToC::EntryT type, const TextSpan& title, DocumentInfo& docInfo ) {
        auto& toc    = docInfo.Get<ToC>();
        auto& styles = docInfo.Get<StyleStack>();
        styles.WriteOpenTag( std::cout, "p", s_classes[(uint32_t)type] );
        auto& cfg = docInfo.Get<Config>(); {
            char chapter[64];
            toc.GetEntryNumber( chapter, type, cfg, title.Top(), title.End() );
            std::cout << chapter << ' ';
        }
        title.WriteTo( std::cout, docInfo,
                       cfg.bTermLinkInHeader ) << "</p>" << std::endl;
    }


} // namespace turnup


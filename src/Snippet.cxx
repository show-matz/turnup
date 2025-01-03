//------------------------------------------------------------------------------
//
// Snippet.cxx
//
//------------------------------------------------------------------------------
#include "Snippet.hxx"

#include "StringReplacer.hxx"
#include "TextMaker.hxx"

#include <iostream>
#include <algorithm>

namespace turnup {

    // snippet の名前と定義行のコレクション
    typedef std::pair<TextSpan, std::vector<TextSpan>* > SnippetInfo;

    static bool IsExpandLine( const TextSpan& line, TextSpan* pParam = nullptr );
    static TextSpan* IsSnippetLine( const TextSpan& line,
                                    TextSpan* pLine, TextSpan* pEnd, TextSpan& name );
    static bool RegisterSnippet( TextSpan* pTop, TextSpan* pEnd, 
                                 const TextSpan& name, std::vector<SnippetInfo>& snippets );
    static const SnippetInfo* FindSnippet( const TextSpan& name,
                                           const std::vector<SnippetInfo>& snippets );
    static void ExpandSnippet( TextSpan params,
                               std::vector<TextSpan>& lines,
                               const std::vector<SnippetInfo>& snippets );
    static void AddErrorLine( std::vector<TextSpan>& lines,
                              const char* msg, const TextSpan& fileName );

    //--------------------------------------------------------------------------
    //
    // implementation of class Snippet
    //
    //--------------------------------------------------------------------------
    void Snippet::Expand( std::vector<TextSpan>& lines ) {

        if( lines.empty() )
            return;

        std::vector<SnippetInfo>    snippets;

        // 行シーケンス全体をスキャンし、snippet 回収と expand 個数調査をする
        TextSpan* const pTop = &(lines[0]);
        TextSpan* const pEnd = pTop + lines.size();
        uint32_t expandCount = 0;
        for( TextSpan* pLine = pTop; pLine < pEnd; ++pLine ) {
            TextSpan line = pLine->Trim();
            if( IsExpandLine( line ) ) {
                ++expandCount;
                continue;
            }
            TextSpan  name;
            TextSpan* pEndOfSnippet = IsSnippetLine( line, pLine, pEnd, name );
            if( pEndOfSnippet ) {
                if( pEndOfSnippet == pEnd ) {
                    std::cerr << "WARNING : end of snippet '" << name <<  "' is missing. " << std::endl;
                } else {
                    if( RegisterSnippet( pLine, pEndOfSnippet, name, snippets ) == false ) {
                        // snippet name collision
                        std::cerr << "WARNING : snippet name '" << name <<  "' duplicated. "
                                  << "First one used." << std::endl;
                    }
                    pLine = pEndOfSnippet;
                }
            }
        }
        // expand が１つ以上ある場合のみ展開処理を実施
        if( 0 < expandCount ) {
            std::vector<TextSpan>  tmpLines;    // snippet 展開後の行シーケンス
            for( TextSpan* pLine = pTop; pLine < pEnd; ++pLine ) {
                TextSpan params;
                if( IsExpandLine( *pLine, &params ) == false )
                    tmpLines.push_back( *pLine );
                else
                    ExpandSnippet( params.Trim(), tmpLines, snippets );
            }
            lines.swap( tmpLines );
        }
        // snippet の一時コンテナを開放
        for( auto& snippet : snippets ) {
            delete snippet.second;
            snippet.second = nullptr;
        }
        snippets.clear();
    }

    //--------------------------------------------------------------------------
    //
    // local functions
    //
    //--------------------------------------------------------------------------
    static bool IsExpandLine( const TextSpan& line, TextSpan* pParam ) {
        TextSpan tmp;
        if( line.IsMatch( "<!-- expand:", tmp, "-->" ) == false )
            return false;
        if( pParam )
            *pParam = tmp;
        return true;
    }

    static TextSpan* IsSnippetLine( const TextSpan& line,
                                    TextSpan* pLine, TextSpan* pEnd, TextSpan& name ) {
        TextSpan tmp;
        if( line.IsMatch( "<!-- snippet:", tmp, "-->" ) ) {
            name = tmp.Trim();
            //[pTop, pEnd) から "<!-- end snippet -->" を探す（ない場合は pEnd 返却）
            return std::find_if( pLine, pEnd,
                                 []( const TextSpan& line ) -> bool {
                                     return line.Trim().IsEqual( "<!-- end snippet -->" );
                                 } );
        }
        if( line.IsMatch( "<!-- snippet:", tmp, "" ) ) {
            //[pTop, pEnd) から "-->" を探して返す（ない場合は pEnd 返却）
            name = tmp.Trim();
            return std::find_if( pLine, pEnd,
                                 []( const TextSpan& line ) -> bool {
                                     return line.Trim().IsEqual( "-->" );
                                 } );
        }
        return nullptr;
    }

    static bool RegisterSnippet( TextSpan* pTop, TextSpan* pEnd, 
                                 const TextSpan& name, std::vector<SnippetInfo>& snippets ) {
        // 同名のスニペットがすでに登録済みであれば何もせずに false 復帰
        if( FindSnippet( name, snippets ) != nullptr )
            return false;
        // TextSpan の vector を新規にアロケート
        auto pContainer = new std::vector<TextSpan>{};
        // 先頭の <!-- snippet: NAME 行をクリアしてスキップ
        pTop->Clear();
        ++pTop;
        // スニペットの本体を vector にコピーしつつ元シーケンスをクリア
        for( ; pTop < pEnd; ++pTop ) {
            pContainer->push_back( TextSpan{ *pTop } );
            pTop->Clear();
        }
        pEnd->Clear();
        // 名前とスニペット本体を snippets に追加して true 復帰
        snippets.push_back( make_pair( name, pContainer ) );
        return true;
    }

    static const SnippetInfo* FindSnippet( const TextSpan& name,
                                           const std::vector<SnippetInfo>& snippets ) {
        //そんなに大量にはならない想定なので線形探索
        auto itr = std::find_if( snippets.begin(), snippets.end(),
                                 [&name]( const SnippetInfo& snippet ) -> bool {
                                     return name.IsEqual( snippet.first );
                                 } );
        if( itr == snippets.end() )
            return nullptr;
        return &*itr;
    } 

    static void ExpandSnippet( TextSpan params,
                               std::vector<TextSpan>& lines,
                               const std::vector<SnippetInfo>& snippets ) {
        TextSpan name = params.CutNextToken();
        params = params.TrimHead();
        const SnippetInfo* pSnippet = FindSnippet( name, snippets );
        if( !pSnippet )
            AddErrorLine( lines, "Snippet is not found : ", name );
        else {
            const std::vector<TextSpan>* pLines = pSnippet->second;
            // パラメータ無しならスニペット本体を単純にコピー
            if( params.IsEmpty() ) {
                std::copy( pLines->begin(),
                           pLines->end(), std::back_inserter( lines ) );
            // パラメータ有りの場合は展開が必要
            } else {
                // パラメータを分解する
                std::vector<TextSpan> prms;
                TextSpan::DestructureToken( params,
                                            []( TextSpan itm, void* p ) -> bool {
                                                ((std::vector<TextSpan>*)p)->push_back( itm );
                                                return true; }, &prms );
                // パラメータ数を９以下に制限（それ以上は単純に無視）
                prms.resize( std::min( 9ul, prms.size() ) );
                // std::transform でもって変換しながら出力
                StringReplacer repl( &(prms[0]), &(prms[0]) + prms.size() );
                std::transform( pLines->begin(), pLines->end(), 
                                std::back_inserter( lines ), repl );
            }
        }
    }

    static void AddErrorLine( std::vector<TextSpan>& lines,
                              const char* msg, const TextSpan& fileName ) {
        TextMaker tm;
        tm << "<!-- error: " << msg << fileName << " -->";
        lines.push_back( tm.GetSpan() );
    }


} // namespace turnup

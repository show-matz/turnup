//------------------------------------------------------------------------------
//
// SimpleFormula.cxx
//
//------------------------------------------------------------------------------
#include "SimpleFormula.hxx"

#include "Utilities.hxx"

#include <assert.h>
#include <string.h>
#include <algorithm>

struct CharMap {
    char        key;
    uint32_t    dataLen;
    const char* data;
};

struct StringMap {
    char        top;
    const char* key;
    const char* data;
    uint32_t    keyLen;
    uint32_t    dataLen;
};


static bool s_moduleInitialized = false;

static CharMap s_alphabetData[] = {
    { 'A', 0, "𝐴" },
    { 'B', 0, "𝐵" },
    { 'C', 0, "𝐶" },
    { 'D', 0, "𝐷" },
    { 'E', 0, "𝐸" },
    { 'F', 0, "𝐹" },
    { 'G', 0, "𝐺" },
    { 'H', 0, "𝐻" },
    { 'I', 0, "𝐼" },
    { 'J', 0, "𝐽" },
    { 'K', 0, "𝐾" },
    { 'L', 0, "𝐿" },
    { 'M', 0, "𝑀" },
    { 'N', 0, "𝑁" },
    { 'O', 0, "𝑂" },
    { 'P', 0, "𝑃" },
    { 'Q', 0, "𝑄" },
    { 'R', 0, "𝑅" },
    { 'S', 0, "𝑆" },
    { 'T', 0, "𝑇" },
    { 'U', 0, "𝑈" },
    { 'V', 0, "𝑉" },
    { 'W', 0, "𝑊" },
    { 'X', 0, "𝑋" },
    { 'Y', 0, "𝑌" },
    { 'Z', 0, "𝑍" },
    { 'a', 0, "𝑎" },
    { 'b', 0, "𝑏" },
    { 'c', 0, "𝑐" },
    { 'd', 0, "𝑑" },
    { 'e', 0, "𝑒" },
    { 'f', 0, "𝑓" },
    { 'g', 0, "𝑔" },
    { 'h', 0, "ℎ" },
    { 'i', 0, "𝑖" },
    { 'j', 0, "𝑗" },
    { 'k', 0, "𝑘" },
    { 'l', 0, "𝑙" },
    { 'm', 0, "𝑚" },
    { 'n', 0, "𝑛" },
    { 'o', 0, "𝑜" },
    { 'p', 0, "𝑝" },
    { 'q', 0, "𝑞" },
    { 'r', 0, "𝑟" },
    { 's', 0, "𝑠" },
    { 't', 0, "𝑡" },
    { 'u', 0, "𝑢" },
    { 'v', 0, "𝑣" },
    { 'w', 0, "𝑤" },
    { 'x', 0, "𝑥" },
    { 'y', 0, "𝑦" },
    { 'z', 0, "𝑧" }
};

static CharMap s_supData[] = {
    { '(', 0, "⁽" },
    { ')', 0, "⁾" },
    { '+', 0, "⁺" },
    { '-', 0, "⁻" },
    { '0', 0, "⁰" },
    { '1', 0, "¹" },
    { '2', 0, "²" },
    { '3', 0, "³" },
    { '4', 0, "⁴" },
    { '5', 0, "⁵" },
    { '6', 0, "⁶" },
    { '7', 0, "⁷" },
    { '8', 0, "⁸" },
    { '9', 0, "⁹" },
    { '=', 0, "⁼" },
    { '^', 0, "^" },
    { 'i', 0, "ⁱ" },
    { 'n', 0, "ⁿ" }
};

static CharMap s_subData[] = {
    { '(', 0, "₍" },
    { ')', 0, "₎" },
    { '+', 0, "₊" },
    { '-', 0, "₋" },
    { '0', 0, "₀" },
    { '1', 0, "₁" },
    { '2', 0, "₂" },
    { '3', 0, "₃" },
    { '4', 0, "₄" },
    { '5', 0, "₅" },
    { '6', 0, "₆" },
    { '7', 0, "₇" },
    { '8', 0, "₈" },
    { '9', 0, "₉" },
    { '=', 0, "₌" },
    { '_', 0, "_" },
    { 'a', 0, "ₐ" },
    { 'e', 0, "ₑ" },
    { 'h', 0, "ₕ" },
    { 'k', 0, "ₖ" },
    { 'l', 0, "ₗ" },
    { 'm', 0, "ₘ" },
    { 'n', 0, "ₙ" },
    { 'o', 0, "ₒ" },
    { 'p', 0, "ₚ" },
    { 's', 0, "ₛ" },
    { 't', 0, "ₜ" },
    { 'x', 0, "ₓ" }
};


// 「最長一致」を検索する必要性から先頭文字のみで sort して std::equal_range を使う
// MEMO : 管理上の理由からソートは実行時に実施する
static StringMap s_otherData[] = {
    { 0, "mathbb C",       "ℂ", 0, 0 },
    { 0, "mathbb H",       "ℍ", 0, 0 },
    { 0, "mathbb N",       "ℕ", 0, 0 },
    { 0, "mathbb P",       "ℙ", 0, 0 },
    { 0, "mathbb Q",       "ℚ", 0, 0 },
    { 0, "mathbb R",       "ℝ", 0, 0 },
    { 0, "mathbb Z",       "ℤ", 0, 0 },
    { 0, "alpha",          "α", 0, 0 },
    { 0, "beta",           "β", 0, 0 },
    { 0, "gamma",          "γ", 0, 0 },
    { 0, "delta",          "δ", 0, 0 },
    { 0, "epsilon",        "ϵ", 0, 0 },
    { 0, "varepsilon",     "ε", 0, 0 },
    { 0, "zeta",           "ζ", 0, 0 },
    { 0, "eta",            "η", 0, 0 },
    { 0, "theta",          "θ", 0, 0 },
    { 0, "vartheta",       "ϑ", 0, 0 },
    { 0, "iota",           "ι", 0, 0 },
    { 0, "kappa",          "κ", 0, 0 },
    { 0, "lambda",         "λ", 0, 0 },
    { 0, "mu",             "μ", 0, 0 },
    { 0, "nu",             "ν", 0, 0 },
    { 0, "xi",             "ξ", 0, 0 },
    { 0, "o",              "𝑜", 0, 0 },
    { 0, "pi",             "π", 0, 0 },
    { 0, "varpi",          "ϖ", 0, 0 },
    { 0, "rho",            "ρ", 0, 0 },
    { 0, "varrho",         "ϱ", 0, 0 },
    { 0, "sigma",          "σ", 0, 0 },
    { 0, "varsigma",       "ς", 0, 0 },
    { 0, "tau",            "τ", 0, 0 },
    { 0, "upsilon",        "υ", 0, 0 },
    { 0, "phi",            "φ", 0, 0 },
    { 0, "varphi",         "ϕ", 0, 0 },
    { 0, "chi",            "χ", 0, 0 },
    { 0, "psi",            "ψ", 0, 0 },
    { 0, "omega",          "ω", 0, 0 },
    { 0, "sin",            "sin", 0, 0 },
    { 0, "cos",            "cos", 0, 0 },
    { 0, "tan",            "tan", 0, 0 },
    { 0, "lim",            "lim", 0, 0 },
    { 0, "log",            "log", 0, 0 },
    { 0, "inf",            "inf", 0, 0 },
    { 0, "sup",            "sup", 0, 0 },
    { 0, "mod",            "mod", 0, 0 },
    { 0, "equiv",          "≡", 0, 0 },
    { 0, "not\\in",        "∉", 0, 0 },
    { 0, "in",             "∈", 0, 0 },
    { 0, "emptyset",       "∅", 0, 0 },
    { 0, "forall",         "∀", 0, 0 },
    { 0, "exists",         "∃", 0, 0 },
    { 0, "Leftarrow",      "⇐", 0, 0 },
    { 0, "Rightarrow",     "⇒", 0, 0 },
    { 0, "Leftrightarrow", "⇔", 0, 0 },
    { 0, "Pi",             "∏", 0, 0 },
    { 0, "Sigma",          "∑", 0, 0 },
    { 0, "infty",          "∞", 0, 0 },
    { 0, "mp",             "∓", 0, 0 },
    { 0, "pm",             "±", 0, 0 },
    { 0, "setminus",       "∖", 0, 0 },
    { 0, "ast",            "∗", 0, 0 },
    { 0, "circ",           "∘", 0, 0 },
    { 0, "cdot",           "∙", 0, 0 },
    { 0, "sqrt",           "√", 0, 0 },
    { 0, "angle",          "∠", 0, 0 },
    { 0, "land",           "∧", 0, 0 },
    { 0, "lor",            "∨", 0, 0 },
    { 0, "cap",            "∩", 0, 0 },
    { 0, "cup",            "∪", 0, 0 },
    { 0, "int",            "∫", 0, 0 },
    { 0, "therefore",      "∴", 0, 0 },
    { 0, "because",        "∵", 0, 0 },
    { 0, "times",          "×", 0, 0 },
    { 0, "div",            "÷", 0, 0 },
    { 0, "sim",            "∼", 0, 0 },
    { 0, "neq",            "≠", 0, 0 },
    { 0, "leq",            "≤", 0, 0 },
    { 0, "geq",            "≥", 0, 0 },
    { 0, "leqq",           "≦", 0, 0 },
    { 0, "geqq",           "≧", 0, 0 },
    { 0, "subset",         "⊂", 0, 0 },
    { 0, "supset",         "⊃", 0, 0 },
    { 0, "subseteq",       "⊆", 0, 0 },
    { 0, "supseteq",       "⊇", 0, 0 },
    { 0, "oplus",          "⊕", 0, 0 },
    { 0, "ominus",         "⊖", 0, 0 },
    { 0, "otimes",         "⊗", 0, 0 },
    { 0, "oslash",         "⊘", 0, 0 }
};


static inline bool operator<( const CharMap& m1, const CharMap& m2 ) {
    return m1.key < m2.key;
}

static inline bool operator<( const StringMap& m1, const StringMap& m2 ) {
    return m1.top < m2.top;
}

static void InitializeModule() {
    auto init1 = []( CharMap& map ) -> void {
        map.dataLen = ::strlen( map.data );
    };
    auto init2 = []( StringMap& map ) -> void {
        map.top     = map.key[0];
        map.keyLen  = ::strlen( map.key  );
        map.dataLen = ::strlen( map.data );
    };
    for( auto& e : s_alphabetData )
        init1( e );
    for( auto& e : s_supData )
        init1( e );
    for( auto& e : s_subData )
        init1( e );
    for( auto& e : s_otherData )
        init2( e );
    std::sort( s_otherData, s_otherData + turnup::count_of(s_otherData) );
    s_moduleInitialized = true;
}




namespace turnup {

    //--------------------------------------------------------------------------
    //
    // implementation of class SimpleFormula
    //
    //--------------------------------------------------------------------------
    void SimpleFormula::Expand( const char* p1, const char* p2,
                                CallbackFunc* pCallback, void* dum ) {
        if( !s_moduleInitialized ) {
            InitializeModule();
        }
        auto opChar = [&pCallback, &dum]( const char* p1, const char* p2,
                                          const CharMap* pMap1, const CharMap* pMap2 ) -> const char* {
            if( *p1 != '{' ) {
                auto itr = std::lower_bound( pMap1, pMap2, CharMap{ *p1, 0, nullptr } );
                if( itr == pMap2 || itr->key != *p1 )
                    return p1;
                pCallback( itr->data, itr->dataLen, dum );
                return p1 + 1;
            }
            auto p = std::find( p1+1, p2, '}' );
            if( p == p2 )
                return p1;
            for( ++p1; p1 < p; ++p1 ) {
                auto itr = std::lower_bound( pMap1, pMap2, CharMap{ *p1, 0, nullptr } );
                if( itr != pMap2 && itr->key == *p1 )
                    pCallback( itr->data, itr->dataLen, dum );
            }
            return p + 1;
        };
        while( p1 < p2 ) {
            if( *p1 == '\\' ) {
                auto pr = std::equal_range( s_otherData,
                                            s_otherData + turnup::count_of(s_otherData),
                                            StringMap{ p1[1], nullptr, nullptr, 0, 0 } );
                const StringMap* pCand = nullptr;
                for( const StringMap* p = pr.first; p < pr.second; ++p ) {
                    if( !::strncmp( p1 + 1, p->key, p->keyLen ) ) {
                        if( !pCand || pCand->keyLen < p->keyLen )
                            pCand = p;
                    }
                }
                if( !pCand )
                    pCallback( p1++, 1, dum );
                else {
                    pCallback( pCand->data, pCand->dataLen, dum );
                    p1 += pCand->keyLen + 1;
                }
                continue;
            }
            if( ('a' <= *p1 && *p1 <= 'z') || ('A' <= *p1 && *p1 <= 'Z') ) {
                p1 = opChar( p1, p2, s_alphabetData,
                                     s_alphabetData + count_of(s_alphabetData) );
                continue;
            }
            if( *p1 == '^' && 1 < (p2 - p1) ) {
                p1 = opChar( p1+1, p2, s_supData, s_supData + count_of(s_supData) );
                continue;
            }
            if( *p1 == '_' && 1 < (p2 - p1) ) {
                p1 = opChar( p1+1, p2, s_subData, s_subData + count_of(s_subData) );
                continue;
            }
            pCallback( p1, 1, dum );
            ++p1;
        }
    }

} // namespace turnup





// g++ --std=c++11 -Wall -Wextra -DSIMPLEFORMULA_TEST -g SimpleFormula.cxx -o SimpleFormula.test.exe
#ifdef SIMPLEFORMULA_TEST

    #include <iostream>

    int main( int argc, char* argv[] ) {

        (void)argc;
        const char* p = argv[1];

        auto callback = []( const char* p, uint32_t len, void* dum ) -> void {
            (void)dum;
            std::cout.write( p, len );
        };

        turnup::SimpleFormula::Expand( p, p + ::strlen( p ), callback, nullptr );
        std::cout << std::endl;

        return 0;
    }

#endif // SIMPLEFORMULA_TEST

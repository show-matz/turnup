//------------------------------------------------------------------------------
//
// ImageModeStack.cxx
//
//------------------------------------------------------------------------------
#include "ImageModeStack.hxx"

#include <vector>

namespace turnup {

    //--------------------------------------------------------------------------
    //
    // class ImageModeStack::Impl
    //
    //--------------------------------------------------------------------------
    class ImageModeStack::Impl {
    private:
        typedef std::vector<ImageMode>  StackEntryList;
    public:
        Impl();
        ~Impl();
    public:
        void PushMode( ImageMode mode );
        bool PopMode();
        ImageMode GetMode() const;
    private:
        StackEntryList    m_entries;
    };

    //--------------------------------------------------------------------------
    //
    // implementation of class ImageModeStack
    //
    //--------------------------------------------------------------------------
    ImageModeStack::ImageModeStack() : m_pImpl( nullptr ) {
    }

    ImageModeStack::~ImageModeStack() {
        delete m_pImpl;
    }

    void ImageModeStack::PushMode( ImageMode mode ) {
        if( !m_pImpl )
            m_pImpl = new Impl{};
        return m_pImpl->PushMode( mode );
    }

    bool ImageModeStack::PopMode() {
        if( !m_pImpl )
            return false;
        return m_pImpl->PopMode();
    }

    ImageMode ImageModeStack::GetCurrentMode() const {
        if( !m_pImpl )
            return ImageMode::LINK;
        return m_pImpl->GetMode();
    }


    //--------------------------------------------------------------------------
    //
    // implementation of class ImageModeStack::Impl
    //
    //--------------------------------------------------------------------------
    ImageModeStack::Impl::Impl() : m_entries( {} ) {
    }

    ImageModeStack::Impl::~Impl() {
        m_entries.clear();
    }

    void ImageModeStack::Impl::PushMode( ImageMode mode ) {
        m_entries.push_back( mode );
    }

    bool ImageModeStack::Impl::PopMode() {
        if( 0 < m_entries.size() ) {
            m_entries.pop_back();
            return true;
        }
        return true;
    }

    ImageMode ImageModeStack::Impl::GetMode() const {
        //エントリ一覧が空なら ImageMode::LINK 復帰
        if( m_entries.empty() )
            return ImageMode::LINK;
        //上記以外なら最後の要素を返却
        return m_entries.back();
    }


} // namespace turnup


#include <iostream>
#include <tchar.h>
#include "WindowsApplication.hpp"
#include "D2D/D2DGraphicsManager.hpp"
#include "MemoryManager.hpp"
#include "AssetLoader.hpp"
#include "SceneManager.hpp"
#include "Utility.hpp"
#include "BMP.hpp"
#include "JPEG.hpp"
#include "PNG.hpp"

using namespace Panda;
using namespace std;

namespace Panda {
    class TestGraphicsManager : public D2DGraphicsManager
    {
        public:
            using D2DGraphicsManager::D2DGraphicsManager;
            void DrawImage(const Image image);
        private:
            ID2D1Bitmap* m_pBitmap = nullptr;
    };

    class TestApplication : public WindowsApplication
    {
    public:
        using WindowsApplication::WindowsApplication;

        virtual int Initialize();

        virtual void OnDraw();

    private:
        Image m_Image;
    };
}

namespace Panda {
	Handness g_ViewHandness = Handness::kHandnessRight;
	DepthClipSpace g_DepthClipSpace = DepthClipSpace::kDepthClipNegativeOneToOne;

    GfxConfiguration config(8, 8, 8, 8, 24, 8, 0, 1024, 512, "Texture Load Test (Windows)");
	IApplication* g_pApp                = static_cast<IApplication*>(new TestApplication(config));
    GraphicsManager* g_pGraphicsManager = static_cast<GraphicsManager*>(new TestGraphicsManager);
    MemoryManager*   g_pMemoryManager   = static_cast<MemoryManager*>(new MemoryManager);
    AssetLoader* g_pAssetLoader = static_cast<AssetLoader*>(new AssetLoader);
    SceneManager* g_pSceneManager = static_cast<SceneManager*>(new SceneManager);
    InputManager*    g_pInputManager    = static_cast<InputManager*>(new InputManager);
}

int Panda::TestApplication::Initialize()
{
    int result;

    result = WindowsApplication::Initialize();

    if (result == 0) {
        Buffer buf;

        PngParser pngParser;
        if(m_ArgC > 1)
        {
            buf = g_pAssetLoader->SyncOpenAndReadBinary(m_ppArgV[1]);
        }
        else 
        {
            buf = g_pAssetLoader->SyncOpenAndReadBinary("Textures/eye.png");
        }

        m_Image = pngParser.Parse(buf);
    }

    return result;
}

void Panda::TestApplication::OnDraw()
{
    dynamic_cast<TestGraphicsManager*>(g_pGraphicsManager)->DrawImage(m_Image);
}

void Panda::TestGraphicsManager::DrawImage(const Image image)
{
	HRESULT hr;

    // start build GPU draw command
    m_pRenderTarget->BeginDraw();

    D2D1_BITMAP_PROPERTIES props;
    props.pixelFormat.format = DXGI_FORMAT_R8G8B8A8_UNORM;
    props.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
    props.dpiX = 72.0f;
    props.dpiY = 72.0f;
    SafeRelease(&m_pBitmap);
    hr = m_pRenderTarget->CreateBitmap(D2D1::SizeU(image.Width, image.Height), 
                                                    image.Data, image.Pitch, props, &m_pBitmap);

    D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();
    D2D1_SIZE_F bmpSize = m_pBitmap->GetSize();

    D2D1_RECT_F source_rect = D2D1::RectF(
                     0,
                     0,
                     bmpSize.width,
                     bmpSize.height
                     );

    float aspect = bmpSize.width / bmpSize.height;
	float dest_height = rtSize.height;
	float dest_width = rtSize.height * aspect;

    D2D1_RECT_F dest_rect = D2D1::RectF(
                     0,
                     0,
                     dest_width,
                     dest_height 
                     );

    m_pRenderTarget->DrawBitmap(m_pBitmap, dest_rect, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, source_rect);

    // end GPU draw command building
    m_pRenderTarget->EndDraw();
}



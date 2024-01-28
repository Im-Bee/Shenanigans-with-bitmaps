#include "Pch.h"

#include "Application.hpp"
#include "UserInputOutput/Output.hpp"
#include "UserInputOutput/Input.hpp"
#include "HexEditor.hpp"

// -----------------------------------------------------------------------------
void Application::Initialize()
{
    SWInputOutput::OutputTextFromResources(IDS_STRING_PROMPT_WELCOME);
    std::wcout << L"Availble commands\n\
        - [q] for quit\n\
        - [load / l] to load a file from path\n\
        - [save / s] to save file in output dir\n\
        - [color] to color whole\n\
        - [lookat] to view file as dec \n\
        - [gray] to make file gray scale \n\
        - [negative] to make file negative\n";

    FindPathToItself();
    CreateSaveDir();
}

// -----------------------------------------------------------------------------
#define SW_IS_BITMAP                                        \
if (!m_pLoadedBitmap.get() ||                               \
    !m_pLoadedBitmap->IsValid())                            \
{                                                           \
    std::cout << "There is no file" << std::endl;           \
    return;                                                 \
}

// -----------------------------------------------------------------------------
void Application::Update()
{
    std::wstring r = SWInputOutput::InputString(L"... : ", true);
    
    // Normalize
    std::for_each(r.begin(), r.end(), [](wchar_t& c) {
        c = std::tolower(c);
        });

    if (r == L"q")
    {
        m_bQuit = true;
        return;
    }
    if (r == L"load" ||
        r == L"l")
    {
        LoadFile();
        return;
    }
    if (r == L"save" ||
        r == L"s")
    {
        
        SW_IS_BITMAP;
        SaveFile();
        return;
    }
    if (r == L"lookat")
    {
        SW_IS_BITMAP;
        LookAtFile();
        return;
    }
    if (r == L"color")
    {
        SW_IS_BITMAP;
        m_pLoadedBitmap->ColorWhole({ 250, 170, 15 });
        return;
    }
    if (r == L"negative")
    {
        SW_IS_BITMAP;
        m_pLoadedBitmap->MakeItNegative();
        return;
    }
    if (r == L"gray")
    {
        SW_IS_BITMAP;
        m_pLoadedBitmap->MakeItGrayScale();
        return;
    }

    if (r == L"ds")
    {
        SW_IS_BITMAP;
        m_pLoadedBitmap->DeleteShadows();
        return;
    }

    std::wcout << L"Invalid command" << std::endl;
}

// -----------------------------------------------------------------------------
void Application::Destroy()
{
    if (m_pLoadedBitmap.get())
        m_pLoadedBitmap->Destroy();
}

// Private ---------------------------------------------------------------------

// -----------------------------------------------------------------------------
void Application::LoadFile()
{
    if (m_pLoadedBitmap.get())
        m_pLoadedBitmap->Destroy();
    else
        m_pLoadedBitmap = std::make_shared<SWBitmaps::Bitmap>();

    SWInputOutput::OutputTextFromResources(IDS_STRING_PROMPT_DOING_BITMAP);
    std::wstring p = SWInputOutput::InputString(L"Set path to target bitmap:");

    // Remove all of '"'
    while (p.find('"') != std::string::npos)
    {
        auto at = p.find_first_of('"');
        p.erase(at, at + 1);
    }

    m_pLoadedBitmap->Initialize(p);
}   

// -----------------------------------------------------------------------------
void Application::SaveFile()
{
    m_pLoadedBitmap->SaveToFile(SAVE_DIR 
        + L"Output" 
        + std::to_wstring(m_uBitmapCounter++) 
        + L".bmp");
}

// -----------------------------------------------------------------------------
void Application::LookAtFile()
{
    auto s = SWBytesManipulation::Session();

    s.SetBuffer(m_pLoadedBitmap->_RawPtr(), m_pLoadedBitmap->_RawSize());
    s.Start();
    
    while (s.IsSessionAlive())
    {
        s.UpdateSession();
    }

    // Super secret windows exclusive feature
#ifdef _WIN32
    system("cls");
#endif // _WIN32

    std::wcout << L"Availble commands\n\
        - [q] for quit\n\
        - [load] to load a file from path\n\
        - [save] to save file in output dir\n\
        - [colorhalf] to color half\n\
        - [color] to color whole\n\
        - [lookat] to view file as dec \n\
        - [negative] to make file negative\n";
}

// -----------------------------------------------------------------------------
void Application::FindPathToItself()
{
#ifdef _WIN32
    wchar_t* tmpFileName = (wchar_t*) malloc(sizeof(wchar_t) * MAX_PATH);
    if (!tmpFileName)
        throw std::bad_alloc();

    GetModuleFileName(NULL, tmpFileName, MAX_PATH);

    wchar_t* lastSlash = wcsrchr(tmpFileName, L'\\');
    *(lastSlash + 1) = L'\0';

    m_PathToItself = tmpFileName;
    free(tmpFileName);

    return;
#endif // DEBUG
    throw;
}

// -----------------------------------------------------------------------------
void Application::CreateSaveDir()
{
#ifdef _WIN64
    CreateDirectory(SAVE_DIR.c_str(), NULL);

    return;
#endif // _WIN64

    throw;
}

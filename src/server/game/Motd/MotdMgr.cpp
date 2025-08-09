/*
 src\server\game\Motd\MotdMgr.cpp
*/

#include "MotdMgr.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

MotdMgr &MotdMgr::Instance()
{
    static MotdMgr instance;
    return instance;
}

MotdMgr::MotdMgr()
{
    // Na construção, tentamos carregar automaticamente (não fatal se falhar)
    Load();
}

bool MotdMgr::Load()
{
    try
    {
        std::ifstream in(m_filePath, std::ios::in);
        if (!in.is_open())
        {
            // Arquivo não existe — motd fica vazio (sem obrigatoriedade)
            m_motd.clear();
            return false;
        }

        std::ostringstream ss;
        ss << in.rdbuf();
        m_motd = ss.str();

        // Remover eventual CR final que cause exibição estranha
        if (!m_motd.empty() && m_motd.back() == '\r')
            m_motd.pop_back();

        in.close();
        return true;
    }
    catch (const std::exception &e)
    {
        // Falha ao ler — deixar motd vazio e retornar false
        std::cerr << "[MotdMgr] Falha ao carregar motd: " << e.what() << "\n";
        m_motd.clear();
        return false;
    }
}

bool MotdMgr::Save() const
{
    try
    {
        // Certificar que diretório existe
        std::filesystem::path p = m_filePath;
        if (p.has_parent_path())
            std::filesystem::create_directories(p.parent_path());

        std::ofstream out(m_filePath, std::ios::out | std::ios::trunc);
        if (!out.is_open())
        {
            std::cerr << "[MotdMgr] Falha ao abrir arquivo para salvar motd: " << m_filePath << "\n";
            return false;
        }

        out << m_motd;
        out.close();
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "[MotdMgr] Exceção ao salvar motd: " << e.what() << "\n";
        return false;
    }
}

void MotdMgr::SetMotd(const std::string &newMotd)
{
    m_motd = newMotd;
    // Salva automaticamente para persistência imediata
    if (!Save())
    {
        std::cerr << "[MotdMgr] Aviso: falha ao salvar motd após SetMotd().\n";
    }
}

const std::string &MotdMgr::GetMotd() const noexcept
{
    return m_motd;
}

void MotdMgr::Clear() noexcept
{
    m_motd.clear();
    // tentamos salvar, mas se falhar não é fatal
    try
    {
        (void)Save();
    }
    catch (...)
    {
    }
}
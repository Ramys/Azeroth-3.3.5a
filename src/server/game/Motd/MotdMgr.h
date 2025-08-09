#pragma once

/*
 src\server\game\Motd\MotdMgr.h
*/

#include <string>

/// MotdMgr
/// Agora o MOTD é opcional (pode estar vazio) e totalmente editável.
/// O conteúdo é carregado/salvo em data/motd.txt por simplicidade.
class MotdMgr
{
public:
    // Singleton accessor
    static MotdMgr &Instance();

    // Carrega o MOTD do arquivo (retorna true se o arquivo foi lido, false se não existia)
    bool Load();

    // Salva o MOTD para o arquivo
    bool Save() const;

    // Define novo MOTD (pode ser string vazia para remover o MOTD)
    void SetMotd(const std::string &newMotd);

    // Retorna o MOTD atualmente em memória
    const std::string &GetMotd() const noexcept;

    // Limpa (remove) o MOTD atual
    void Clear() noexcept;

private:
    MotdMgr();
    ~MotdMgr() = default;

    // Path do arquivo onde o MOTD é salvo/ carregado
    const std::string m_filePath = "data/motd.txt";

    // Conteúdo atual do MOTD (pode ser vazio)
    std::string m_motd;

    // Não copiável
    MotdMgr(const MotdMgr &) = delete;
    MotdMgr &operator=(const MotdMgr &) = delete;
};
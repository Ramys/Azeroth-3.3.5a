static bool HandleBanCharacterCommand(ChatHandler* handler, char const* args)
{
    if (!*args)
        return false;

    char* nameStr = strtok((char*)args, " ");
    if (!nameStr)
        return false;

    std::string name = nameStr;

    char* durationStr = strtok(nullptr, " ");
    if (!durationStr || !atoi(durationStr))
        return false;

    char* reasonStr = strtok(nullptr, "");
    if (!reasonStr)
        return false;

    if (!normalizePlayerName(name))
    {
        handler->SendErrorMessage(LANG_PLAYER_NOT_FOUND);
        return false;
    }

    switch (sBan->BanCharacter(name, durationStr, reasonStr, handler->GetSession() ? handler->GetSession()->GetPlayerName() : ""))
    {
        case BAN_SUCCESS:
            if (atoi(durationStr) > 0)
            {
                if (!sWorld->getBoolConfig(CONFIG_SHOW_BAN_IN_WORLD))
                    handler->PSendSysMessage(LANG_BAN_YOUBANNED, name, secsToTimeString(TimeStringToSecs(durationStr), true), reasonStr);
                // Mensagem global para todos os jogadores
                sWorld->SendWorldText("O jogador %s foi banido por %s. Motivo: %s", name.c_str(), secsToTimeString(TimeStringToSecs(durationStr), true).c_str(), reasonStr);
            }
            else
            {
                if (!sWorld->getBoolConfig(CONFIG_SHOW_BAN_IN_WORLD))
                    handler->PSendSysMessage(LANG_BAN_YOUPERMBANNED, name, reasonStr);
                // Mensagem global para todos os jogadores
                sWorld->SendWorldText("O jogador %s foi banido permanentemente. Motivo: %s", name.c_str(), reasonStr);
            }
            break;
        case BAN_NOTFOUND:
            {
                handler->SendErrorMessage(LANG_BAN_NOTFOUND, "character", name);
                return false;
            }
        default:
            break;
    }

    return true;
}

static bool HandleBanHelper(BanMode mode, char const* args, ChatHandler* handler)
{
    if (!*args)
        return false;

    char* cnameOrIP = strtok((char*)args, " ");
    if (!cnameOrIP)
        return false;

    std::string nameOrIP = cnameOrIP;

    char* durationStr = strtok(nullptr, " ");
    if (!durationStr || !atoi(durationStr))
        return false;

    char* reasonStr = strtok(nullptr, "");
    if (!reasonStr)
        return false;

    switch (mode)
    {
        case BAN_ACCOUNT:
            if (!Utf8ToUpperOnlyLatin(nameOrIP))
            {
                handler->SendErrorMessage(LANG_ACCOUNT_NOT_EXIST, nameOrIP);
                return false;
            }
            break;
        case BAN_CHARACTER:
            if (!normalizePlayerName(nameOrIP))
            {
                handler->SendErrorMessage(LANG_PLAYER_NOT_FOUND);
                return false;
            }
            break;
        case BAN_IP:
            if (!IsIPAddress(nameOrIP.c_str()))
                return false;
            break;
    }

    BanReturn banReturn;

    switch (mode)
    {
        case BAN_ACCOUNT:
            banReturn = sBan->BanAccount(nameOrIP, durationStr, reasonStr, handler->GetSession() ? handler->GetSession()->GetPlayerName() : "Console");
            break;
        case BAN_CHARACTER:
            banReturn = sBan->BanAccountByPlayerName(nameOrIP, durationStr, reasonStr, handler->GetSession() ? handler->GetSession()->GetPlayerName() : "Console");
            break;
        case BAN_IP:
        default:
            banReturn = sBan->BanIP(nameOrIP, durationStr, reasonStr, handler->GetSession() ? handler->GetSession()->GetPlayerName() : "Console");
            break;
    }

    switch (banReturn)
    {
        case BAN_SUCCESS:
            if (atoi(durationStr) > 0)
            {
                if (!sWorld->getBoolConfig(CONFIG_SHOW_BAN_IN_WORLD))
                    handler->PSendSysMessage(LANG_BAN_YOUBANNED, nameOrIP, secsToTimeString(TimeStringToSecs(durationStr), true), reasonStr);
                // Mensagem global para todos os jogadores
                sWorld->SendWorldText("O jogador/IP %s foi banido por %s. Motivo: %s", nameOrIP.c_str(), secsToTimeString(TimeStringToSecs(durationStr), true).c_str(), reasonStr);
            }
            else
            {
                if (!sWorld->getBoolConfig(CONFIG_SHOW_BAN_IN_WORLD))
                    handler->PSendSysMessage(LANG_BAN_YOUPERMBANNED, nameOrIP, reasonStr);
                // Mensagem global para todos os jogadores
                sWorld->SendWorldText("O jogador/IP %s foi banido permanentemente. Motivo: %s", nameOrIP.c_str(), reasonStr);
            }
            break;
        case BAN_SYNTAX_ERROR:
            return false;
        case BAN_NOTFOUND:
            switch (mode)
            {
                default:
                    handler->SendErrorMessage(LANG_BAN_NOTFOUND, "account", nameOrIP);
                    break;
                case BAN_CHARACTER:
                    handler->SendErrorMessage(LANG_BAN_NOTFOUND, "character", nameOrIP);
                    break;
                case BAN_IP:
                    handler->SendErrorMessage(LANG_BAN_NOTFOUND, "ip", nameOrIP);
                    break;
            }
            return false;
        case BAN_LONGER_EXISTS:
            handler->PSendSysMessage("Unsuccessful! A longer ban is already present on this account!");
            break;
        default:
            break;
    }

    return true;
}
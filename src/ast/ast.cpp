#include <cstring>
#include <iostream>

#include "ast.h"

std::ostream &
operator<<(std::ostream &stream, const CXString &str)
{
    stream << clang_getCString(str);
    clang_disposeString(str);
    return stream;
}

Ast::Ast()
{
    m_index = clang_createIndex(0, 0);
}

Ast::~Ast()
{
    clang_disposeIndex(m_index);
}

void Ast::parseFile(const std::string &filename)
{
    auto unit = clang_parseTranslationUnit(m_index, filename.c_str(), nullptr, 0, nullptr, 0, CXTranslationUnit_None);

    if (unit == nullptr)
    {
        std::cerr << "Ast::parseFile(): unable to parse \"" << filename << "\"." << std::endl;
    }
    else
    {
        CXCursor cursor = clang_getTranslationUnitCursor(unit);
        clang_visitChildren(cursor,
            [](CXCursor c, CXCursor parent, CXClientData client_data)
            {
                CXCursorKind kind =  clang_getCursorKind(c);
                if (kind == 4   ||
                    kind == 6   ||
                    kind == 8   ||
                    kind == 9   ||
                    kind == 21  ||
                    kind == 43  ||
                    kind == 101 ||
                    kind == 102 ||
                    kind == 103 ||
                    kind == 43)
                if (clang_getCursorKind(c) != 501)
                {
                    CXString cursorspelling = clang_getCursorSpelling(c);
                    size_t varlen = strlen(clang_getCString(cursorspelling));
                    if (varlen > 0)
                    {
                        CXFile file;
                        unsigned int line, column, offset;

                        clang_getSpellingLocation(clang_getCursorLocation(c), &file, &line, &column, &offset);

                        if (file)
                            std::cout << "'" << cursorspelling << "' "
                                 << "of kind '" << clang_getCursorKindSpelling(clang_getCursorKind(c)) << "' "
                                 << "(" << clang_getCursorKind(c) << ") "
                                 << clang_getFileName(file) << ":" << line << ":" << column << "-" << column+varlen-1
                                 << std::endl;
                         else
                            std::cout << "'" << cursorspelling << "' "
                                 << "of kind '" << clang_getCursorKindSpelling(clang_getCursorKind(c)) << "' "
                                 << "(" << clang_getCursorKind(c) << ") "
                                 << std::endl;
                    }
                }
                return CXChildVisit_Recurse;
            }, nullptr);
    }

    clang_disposeTranslationUnit(unit);
}

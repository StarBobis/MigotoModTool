#pragma once
#include "D3d11GameType.h"
#include <map>

class VertexDataLine
{
public:

    std::wstring Slot;
    std::wstring Index;
    std::wstring Offset;
    std::wstring ElementName;
    std::wstring ElementValue;

    //example: vb0[0]+000 POSITION: 5.04093075, 4.65782213, 1.05010247
    std::wstring OriginalStrLine;

    VertexDataLine();

    VertexDataLine(const std::wstring inputString);

};


class VertexBufferTxtFile {
public:
    std::wstring FileName;
    std::wstring Index;
    std::wstring Hash;
    std::wstring Stride;
    std::wstring VertexCount = L"";
    std::wstring Topology;
    std::map<int, std::vector<VertexDataLine>> IndexVertexDataLineListMap;

    D3D11GameType d3d11GameType;

    std::vector<std::string> realElementList;
    std::vector<D3D11Element> showElementList;

    void outputVB0File(std::wstring outputFileName);

    VertexBufferTxtFile();


    VertexBufferTxtFile(const std::wstring& filename,D3D11GameType,bool,bool);

    VertexBufferTxtFile(const std::wstring& filename);


    std::vector<std::string> parseRealElementList(const std::wstring& filename,bool);

    std::vector<D3D11Element> parseShowElementList(const std::wstring& filename);

    void parseAllLineFromFile(const std::wstring& filename);

};


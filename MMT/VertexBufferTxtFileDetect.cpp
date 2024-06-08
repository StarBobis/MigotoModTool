#include "VertexBufferTxtFileDetect.h"
#include "GlobalConfigs.h"
#include "VertexBufferTxtFile.h"



VertexBufferDetect::VertexBufferDetect() {

}


VertexBufferDetect::VertexBufferDetect(std::wstring VBFilePath) {
	//根据传入的VBFilePath，获取当前是vb几
	//LOG.LogOutput(L"Try to parse vbSlotNumber: ");
	std::wstring txtFileName = MMTString_GetFileNameFromFilePath(VBFilePath);
	std::wstring vbSlotNumber = txtFileName.substr(9, 1);
	std::wstring currentIndex = txtFileName.substr(0, 6);
	//LOG.Info(L"Current VB SlotNumber: " + vbSlotNumber);
	this->vbSlotNumber = std::stoi(vbSlotNumber);
	this->Index = currentIndex;
	//解析每个Trianglelist的VB文件
	std::wifstream file(VBFilePath);
	if (!file.is_open()) {
		LOG.Error(L"Can't open: " + VBFilePath);
	}
	else {
		//LOG.Info(L"Parsing ElementList For: " + VBFilePath);
	}

	//解析用到的变量
	std::wstring line;

	std::string topology = "";
	int fileShowStride = 0;

	while (std::getline(file, line)) {
		boost::algorithm::to_lower(line);
		boost::algorithm::trim(line);

		// 1.首先解析文件里显示的stride
		std::wstring vertexCountStr = L"vertex count: ";
		std::wstring topologyStr = L"topology: ";
		if (boost::algorithm::starts_with(line, "stride")) {
			std::wstring str;
			std::size_t colonPos = line.find(':');
			if (colonPos != std::wstring::npos) {
				// 截取冒号后面的所有字符，+1 是为了跳过冒号本身
				str = line.substr(colonPos + 1);
				boost::algorithm::trim(str);
			}
			fileShowStride = std::stoi(str);
			this->fileShowStride = fileShowStride;
			//LOG.LogOutput(L"Detect file show stride:" + str);
		}
		else if (boost::algorithm::starts_with(line, topologyStr)) {
			std::wstring valueStr = line.substr(topologyStr.length());
			boost::algorithm::trim(valueStr);
			topology = MMTString_ToByteString(valueStr);
			this->Topology = valueStr;
		}
		else if (boost::algorithm::starts_with(line, vertexCountStr)) {
			std::wstring valueStr = line.substr(vertexCountStr.length());
			boost::algorithm::trim(valueStr);
			this->fileShowVertexCount = std::stoi(valueStr);
		}

		if (boost::algorithm::starts_with(line, "element")) {
			break;
		}
	}
	file.close();


	VertexBufferTxtFile vertexBufferFileData;
	std::vector<D3D11Element> showD3d11ElementList = vertexBufferFileData.parseShowElementList(VBFilePath);
	//LOG.Info(L"Show D3D11Element List: ");
	std::vector<std::string> showD3D11ElementNameList;
	for (D3D11Element showD3d11Element : showD3d11ElementList) {
		//LOG.Info(showD3d11Element.SemanticName);
		showD3D11ElementNameList.push_back(showD3d11Element.SemanticName);
	}
	this->showElementNameList = showD3D11ElementNameList;

	std::vector<std::string> readD3D11ElementNameList = vertexBufferFileData.parseRealElementList(VBFilePath, true);
	//LOG.Info(L"real D3D11Element List: ");
	//for (std::string realD3D11ElementName : readD3D11ElementNameList) {
	//	LOG.Info(MMTString_ToWideString(realD3D11ElementName));
	//}

	//在解析RealElementList时，我们的真实VertexCount就被设置好了，这里要赋值回去
	this->fileRealVertexCount = std::stoi(vertexBufferFileData.VertexCount);

	//LOG.Info(L"Start to add real d3d11Element:");
	std::vector<D3D11Element> realD3D11ElementList;
	for (D3D11Element d3d11Element : showD3d11ElementList) {
		//这里加上SemanticIndex之后的名字，必须出现在readD3D11ElementNameList中才行
		std::string d3d11ElementName = "";
		if (d3d11Element.SemanticIndex != "0") {
			d3d11ElementName = d3d11Element.SemanticName + d3d11Element.SemanticIndex;
		}
		else {
			d3d11ElementName = d3d11Element.SemanticName;
		}

		//遍历列表查找是否存在这个名字，存在则添加并break
		for (std::string elementName : readD3D11ElementNameList) {
			boost::algorithm::to_lower(elementName);
			if (elementName == d3d11ElementName) {
				//LOG.Info(L"Add: " + MMTString_ToWideString(elementName));
				realD3D11ElementList.push_back(d3d11Element);
				break;
			}
		}

	}
	//LOG.Info(L"Add real d3d11Element over.");

	// 这里需要首先根据alignedbyteoffset确定除了最后一个元素的各个元素的步长
	int lastAlignedByteOffset = 0;
	std::string lastSemanticName = "";
	int lastByteWidth = 0;
	std::unordered_map<std::string, int> semanticNameByteWidthMap;
	for (size_t i = 0; i < realD3D11ElementList.size(); i++)
	{
		D3D11Element d3d11Element = realD3D11ElementList[i];
		//LOG.LogOutput(L"Processing: " + d3d11Element.SemanticName);
		if (i == 0) {
			lastSemanticName = d3d11Element.SemanticName;
			lastAlignedByteOffset = d3d11Element.AlignedByteOffset;
			continue;
		}
		else {
			lastByteWidth = d3d11Element.AlignedByteOffset - lastAlignedByteOffset;
			semanticNameByteWidthMap[lastSemanticName] = lastByteWidth;
			lastSemanticName = d3d11Element.SemanticName;
			lastAlignedByteOffset = d3d11Element.AlignedByteOffset;
		}
	}

	//准备根据真实的ElementList列表，求出真实的文件步长
	int fileRealStride = 0;
	if (realD3D11ElementList.size() >= 1) {
		//还有最后一个，根据标记的stride来求出
		D3D11Element lastD3D11Element = realD3D11ElementList[realD3D11ElementList.size() - 1];
		lastByteWidth = fileShowStride - lastD3D11Element.AlignedByteOffset;
		semanticNameByteWidthMap[lastD3D11Element.SemanticName] = lastByteWidth;
		//LOG.LogOutputSplitStr();

		//LOG.LogOutput(L"SemanticName's stride calculated by file read stride: ");
		for (const auto& pair : semanticNameByteWidthMap) {
			std::string semanticName = pair.first;
			int byteWidth = pair.second;
			//LOG.LogOutput(L"SemanticName: " + semanticName + L" ByteWidth: " + std::to_wstring(byteWidth));
		}
		//LOG.LogOutputSplitStr();
		//最后拼接出一个d3d11ElementName-d3d11Element的Map
		//由于文件里显示的Stride不是真正的Stride，所以我们现在需要给每一个
		std::unordered_map<std::string, D3D11Element>  elementNameD3D11ElementMap;

		//这里越完整越好，暂时先这样后续有需要再补充
		std::unordered_map<std::string, int>  d3d11FormatByteWidthMap;
		d3d11FormatByteWidthMap["r32g32b32_float"] = 12;
		d3d11FormatByteWidthMap["r32g32b32a32_float"] = 16;
		d3d11FormatByteWidthMap["r8g8b8a8_unorm"] = 4;
		d3d11FormatByteWidthMap["r8g8b8a8_snorm"] = 4;
		d3d11FormatByteWidthMap["r32g32_float"] = 8;
		d3d11FormatByteWidthMap["r32g32_uint"] = 8;
		d3d11FormatByteWidthMap["r32g32_sint"] = 8;
		d3d11FormatByteWidthMap["r16g16_float"] = 4;
		d3d11FormatByteWidthMap["r16g16_sint"] = 4;
		d3d11FormatByteWidthMap["r16g16_uint"] = 4;
		d3d11FormatByteWidthMap["r32g32b32a32_sint"] = 16;
		d3d11FormatByteWidthMap["r32g32b32a32_uint"] = 16;
		d3d11FormatByteWidthMap["r16g16b16a16_float"] = 8;
		d3d11FormatByteWidthMap["r32_uint"] = 4;

		//LOG.Info(L"Real d3d11 element name byte width: ");
		//遍历d3d11Element列表并赋值
		for (D3D11Element d3d11Element : realD3D11ElementList) {
			// 这里需要我们根据一个d3d11 format和byte width的map来获取
			if (!d3d11FormatByteWidthMap.contains(d3d11Element.Format)) {
				LOG.Error("Unknown d3d11 format" + d3d11Element.Format + " not in d3d11ElementNameByteWidthMap!");
			}
			d3d11Element.ByteWidth = d3d11FormatByteWidthMap[d3d11Element.Format];
			d3d11Element.ExtractTechnique = topology;
			std::string d3d11ElementName = d3d11Element.SemanticName;
			if (d3d11Element.SemanticIndex != "0") {
				d3d11ElementName = d3d11ElementName + d3d11Element.SemanticIndex;
			}

			//LOG.Info("real add real stride ElementName: " + d3d11ElementName + " ByteWidth: " + std::to_string(d3d11Element.ByteWidth));
			elementNameD3D11ElementMap[d3d11ElementName] = d3d11Element;
			this->realElementNameList.push_back(d3d11ElementName);
			this->realElementList.push_back(d3d11Element);
			fileRealStride += d3d11Element.ByteWidth;
		}

		int fileElementListShowStride = 0;
		for (D3D11Element d3d11Element : showD3d11ElementList) {
			// 这里需要我们根据一个d3d11 format和byte width的map来获取
			if (!d3d11FormatByteWidthMap.contains(d3d11Element.Format)) {
				LOG.Error("Unknown d3d11 format" + d3d11Element.Format + " not in d3d11ElementNameByteWidthMap!");
			}
			d3d11Element.ByteWidth = d3d11FormatByteWidthMap[d3d11Element.Format];
			std::string d3d11ElementName = d3d11Element.SemanticName;
			if (d3d11Element.SemanticIndex != "0") {
				d3d11ElementName = d3d11ElementName + d3d11Element.SemanticIndex;
			}
			//LOG.Info(" add show stride ElementName: " + d3d11ElementName + " ByteWidth: " + std::to_string(d3d11Element.ByteWidth));
			fileElementListShowStride += d3d11Element.ByteWidth;
		}

		this->fileRealStride = fileRealStride;
		this->elementNameD3D11ElementMap = elementNameD3D11ElementMap;
	}
	else {
		this->fileRealStride = fileRealStride;
	}

	// 从buf文件中获取对应长度
	
	std::wstring txtFileNamePrefix = txtFileName.substr(0,txtFileName.length() - 4);
	std::wstring bufFileName = txtFileNamePrefix + L".buf";
	std::wstring txtFilePath = MMTString_GetFolderPathFromFilePath(VBFilePath);
	std::wstring bufFilePath = txtFilePath + L"/" + bufFileName;
	// 确保获取到步长
	int countStride = fileShowStride;
	if (countStride == 0) {
		countStride = fileRealStride;
	}
	//LOG.Info(L".buf file path: " + bufFilePath);
	int fileSize = MMTFile_GetFileSize(bufFilePath);
	this->fileBufferVertexCount = fileSize / countStride;
	//LOG.Info(L"file buffer vertex count: " + std::to_wstring(this->fileBufferVertexCount));
	//LOG.NewLine();
	int fileRealSize = MMTFile_GetRealFileSize_NullTerminated(bufFilePath);
	this->fileBufferRealVertexCount = fileRealSize / countStride;


}
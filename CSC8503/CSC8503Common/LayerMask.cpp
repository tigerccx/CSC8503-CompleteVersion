#include "LayerMask.h"
#include "json/json.h"
#include <iostream>
#include <fstream>
#include <string>

namespace NCL{
	namespace CSC8503 {
		LayerMask LayerMask::all = LayerMask(LLONG_MAX);
		std::map<std::string, Layer> Layer::layers = std::map<std::string, Layer>();
		std::map<std::string, LayerMask> Layer::betweenLayerDetections = std::map<std::string, LayerMask>();
		int Layer::layerOffset = 0;
		bool Layer::InitLayerFromJson(const std::string& pathJson)
		{
			std::ifstream ifs(pathJson);

			if (!ifs.is_open())
			{
				std::cout << "Error opening file\n";
				return false;
			}

			Json::Reader reader;
			Json::Value root;

			if (!reader.parse(ifs, root, false))
			{
				std::cerr << "parse failed \n";
				return false;
			}

			Json::Value layerNames = root["layerNames"];
			for (unsigned int i = 0; i < layerNames.size(); ++i) {
				std::string layerName = layerNames[i].asString();
				Layer::AddLayerAndAllDetections(layerName);
			}
			Json::Value layerIgnores = root["ignoredCollisions"];
			for (unsigned int i = 0; i < layerIgnores.size(); ++i) {
				std::string layerName0 = layerIgnores[i]["layer0"].asString();
				std::string layerName1 = layerIgnores[i]["layer1"].asString();
				Layer::RemoveBetweenLayersDetection(layerName0, layerName1);
			}

			ifs.close();
			return true;
		}
	}
}
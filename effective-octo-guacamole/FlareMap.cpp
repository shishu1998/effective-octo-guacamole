
#include "FlareMap.h"
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <cassert>

FlareMap::FlareMap() {
	mapData = std::vector<std::vector<unsigned int>>();
	mapWidth = -1;
	mapHeight = -1;
}

FlareMap::~FlareMap() {
	for(int i=0; i < mapHeight; i++) {
		mapData[i].clear();
	}
	mapData.clear();
}

bool FlareMap::ReadHeader(std::ifstream &stream) {
	std::string line;
	mapWidth = -1;
	mapHeight = -1;
	while(std::getline(stream, line)) {
		if(line == "") { break; }
		std::istringstream sStream(line);
		std::string key,value;
		std::getline(sStream, key, '=');
		std::getline(sStream, value);
		if(key == "width") {
			mapWidth = std::atoi(value.c_str());
		} else if(key == "height"){
			mapHeight = std::atoi(value.c_str());
		}
	}
	if(mapWidth == -1 || mapHeight == -1) {
		return false;
	} else {
		for(int i = 0; i < mapHeight; ++i) {
			mapData.push_back(std::vector<unsigned int>());
		}
		return true;
	}
}

bool FlareMap::ReadLayerData(std::ifstream &stream) {
	std::string line;
	while(getline(stream, line)) {
		if(line == "") { break; }
		std::istringstream sStream(line);
		std::string key,value;
		std::getline(sStream, key, '=');
		std::getline(sStream, value);
		if(key == "data") {
			for(int y=0; y < mapHeight; y++) {
				getline(stream, line);
				std::istringstream lineStream(line);
				std::string tile;
				for(int x=0; x < mapWidth; x++) {
					std::getline(lineStream, tile, ',');
					unsigned int val =  (unsigned int)atoi(tile.c_str());
					if(val > 0) {
						mapData[y].push_back(val-1);
					} else {
						mapData[y].push_back(0);
					}
				}
			}
		}
	}
	return true;
}


bool FlareMap::ReadEntityData(std::ifstream &stream) {
	std::string line;
	std::string type;
	while(getline(stream, line)) {
		if(line == "") { break; }
		std::istringstream sStream(line);
		std::string key,value;
		getline(sStream, key, '=');
		getline(sStream, value);
		if(key == "type") {
			type = value;
		} else if(key == "location") {
			std::istringstream lineStream(value);
			std::string xPosition, yPosition;
			getline(lineStream, xPosition, ',');
			getline(lineStream, yPosition, ',');
			
			FlareMapEntity newEntity;
			newEntity.type = type;
			newEntity.x = std::atoi(xPosition.c_str());
			newEntity.y = std::atoi(yPosition.c_str());
			entities.push_back(newEntity);
		}
	}
	return true;
}

void FlareMap::Load(const std::string fileName) {
	std::ifstream infile(fileName);
	if(infile.fail()) {
		assert(false); // unable to open file
	}
	std::string line;
	while (std::getline(infile, line)) {
		if(line == "[header]") {
			if(!ReadHeader(infile)) {
				assert(false); // invalid file data
			}
		} else if(line == "[layer]") {
			ReadLayerData(infile);
		} else if(line == "[ObjectsLayer]") {
			ReadEntityData(infile);
		}
	}
}

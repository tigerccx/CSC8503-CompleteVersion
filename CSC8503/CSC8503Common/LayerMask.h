#pragma once
#include <string>
#include <vector>
#include <map>
#include <climits>

namespace NCL {
	namespace CSC8503 {

		class LayerMask
		{
		public:
			LayerMask() :layerMask(0) {

			}
			LayerMask(long long int mask) :
				layerMask(mask) {

			}
			long long int layerMask;

			void Clear() {
				layerMask = 0;
			}

			static LayerMask all;
		};

		class Layer {
		public:
			Layer() :
				layer(0), layerName("")
			{
			}
			Layer(long long int l, std::string name):
				layer(l), layerName(name)
			{
			}
		protected:
			long long int layer;
			std::string layerName;

		public:

			static bool InitLayerFromJson(const std::string& pathJson);

			inline long long int GetLayer() const { return layer; }
			inline std::string GetName() const { return layerName; }

			virtual bool operator==(const Layer& other) const {
				return this->layer == other.layer;
			}
			virtual bool operator<(const Layer& other) const {
				return this->layer < other.layer;
			}
			virtual bool operator>(const Layer& other) const {
				return this->layer > other.layer;
			}
			virtual bool operator<=(const Layer& other) const {
				return this->layer <= other.layer;
			}
			virtual bool operator>=(const Layer& other) const {
				return this->layer >= other.layer;
			}
		public:
			static LayerMask Combine(const Layer& a, const Layer& b) {
				return LayerMask(a.layer | b.layer);
			}
			static LayerMask Combine(const LayerMask& a, const Layer& b) {
				return LayerMask(a.layerMask | b.layer);
			}
			static LayerMask Combine(const LayerMask& a, const LayerMask& b) {
				return LayerMask(a.layerMask | b.layerMask);
			}
			static LayerMask Exclude(const LayerMask& a, const Layer& b) {
				return LayerMask(a.layerMask & (~b.layer));
			}
			static LayerMask Exclude(const LayerMask& a, const LayerMask& b) {
				return LayerMask(a.layerMask & (~b.layerMask));
			}
			static bool IsIncludedIn(const LayerMask& mask, const Layer& layer) {
				return (mask.layerMask & layer.layer) > 0;
			}
			static bool IsExcludedFrom(const LayerMask& mask, const Layer& layer) {
				return (mask.layerMask & layer.layer) == 0;
			}

		public:
			static std::map<std::string, Layer> layers;
			static std::map<std::string, LayerMask> betweenLayerDetections;

			static void Init() {
				AddLayerAndAllDetections("Default");
			}

			static bool AddLayer(const std::string& name) {
				if (layers.find(name) != layers.end())
					return false;
				long long int a = 1;
				layers[name] = Layer(a << layerOffset, name);
				layerOffset++;
				return true;
			}

			static bool AddLayerAndAllDetections(const std::string& name) {
				bool rslt = AddLayer(name);
				if (rslt) {
					for (auto itr = layers.begin(); itr != layers.end(); ++itr) {
						AddBetweenLayersDetection(name, itr->first);
					}
				}
				return rslt;
			}

			static bool GetLayer(const std::string& name, Layer& layer) {
				auto itr = layers.find(name);
				if (itr != layers.end()) {
					layer = itr->second;
					return true;
				}
				return false;
			}

			static bool GetLayerMaskFromLayers(const std::vector<std::string>& names, LayerMask& layerMask) {
				layerMask.Clear();
				for (const auto& name : names) {
					auto itr = layers.find(name);
					if (itr != layers.end()) {
						layerMask = Combine(layerMask, itr->second);
					}
					else {
						layerMask.Clear();
						return false;
					}
				}
				return true;
			}

			static bool AddBetweenLayersDetection(std::string nameA, std::string nameB) {
				if (nameA != nameB) {
					Layer layerA, layerB;
					if (!GetLayer(nameA, layerA)) {
						return false;
					}
					if (!GetLayer(nameB, layerB)) {
						return false;
					}

					//Add B to A
					auto itrA = betweenLayerDetections.find(nameA);
					if (itrA != betweenLayerDetections.end()) {
						itrA->second = Combine(itrA->second, layerB);
					}
					else
					{
						betweenLayerDetections[nameA] = LayerMask(layerB.layer);
					}
					//Add A to B
					auto itrB = betweenLayerDetections.find(nameB);
					if (itrB != betweenLayerDetections.end()) {
						itrB->second = Combine(itrB->second, layerA);
					}
					else
					{
						betweenLayerDetections[nameB] = LayerMask(layerA.layer);
					}
				}
				else
				{
					Layer layerA;
					if (!GetLayer(nameA, layerA)) {
						return false;
					}

					//Add to A
					auto itrA = betweenLayerDetections.find(nameA);
					if (itrA != betweenLayerDetections.end()) {
						itrA->second = Combine(itrA->second, layerA);
					}
					else
					{
						betweenLayerDetections[nameA] = LayerMask(layerA.layer);
					}
				}

				return true;
			}

			static bool RemoveBetweenLayersDetection(std::string nameA, std::string nameB) {
				if (nameA != nameB) {
					Layer layerA, layerB;
					if (!GetLayer(nameA, layerA)) {
						return false;
					}
					if (!GetLayer(nameB, layerB)) {
						return false;
					}

					//Remove B from A
					auto itrA = betweenLayerDetections.find(nameA);
					if (itrA != betweenLayerDetections.end()) {
						itrA->second = Exclude(itrA->second, layerB);
					}
					else
					{
						return false;
					}
					//Remove A from B
					auto itrB = betweenLayerDetections.find(nameB);
					if (itrB != betweenLayerDetections.end()) {
						itrB->second = Exclude(itrB->second, layerA);
					}
					else
					{
						return false;
					}
				}
				else
				{
					Layer layerA;
					if (!GetLayer(nameA, layerA)) {
						return false;
					}
					//Remove to A
					auto itrA = betweenLayerDetections.find(nameA);
					if (itrA != betweenLayerDetections.end()) {
						itrA->second = Exclude(itrA->second, layerA);
					}
					else
					{
						return false;
					}
				}

				return true;
			}

			static bool CheckBetweenLayersDetection(std::string nameA, std::string nameB) {
				if (nameA != nameB) {
					Layer layerA, layerB;
					if (!GetLayer(nameA, layerA)) {
						return false;
					}
					if (!GetLayer(nameB, layerB)) {
						return false;
					}

					//Check in A
					auto itrA = betweenLayerDetections.find(nameA);
					if (itrA != betweenLayerDetections.end()) {
						return IsIncludedIn(itrA->second, layerB);
					}
					else
					{
						return false;
					}
				}
				else
				{
					Layer layerA;
					if (!GetLayer(nameA, layerA)) {
						return false;
					}
					//Check in A
					auto itrA = betweenLayerDetections.find(nameA);
					if (itrA != betweenLayerDetections.end()) {
						return IsIncludedIn(itrA->second, layerA);
					}
					else
					{
						return false;
					}
				}
			}

			static bool GetBetweenLayerDetection(std::string name, LayerMask& mask) {
				auto itrA = betweenLayerDetections.find(name);
				if (itrA != betweenLayerDetections.end()) {
					mask = itrA->second;
					return true;
				}
				return false;
			}

		protected:
			static int layerOffset;
		};
	}
}


#include "Debug.h"
#include "../../Common/Matrix4.h"
using namespace NCL;

OGLRenderer* Debug::renderer = nullptr;

std::vector<Debug::DebugStringEntry>	Debug::stringEntries;
std::vector<Debug::DebugLineEntry>		Debug::lineEntries;

const Vector4 Debug::RED	= Vector4(1, 0, 0, 1);
const Vector4 Debug::GREEN	= Vector4(0, 1, 0, 1);
const Vector4 Debug::BLUE	= Vector4(0, 0, 1, 1);

const Vector4 Debug::BLACK	= Vector4(0, 0, 0, 1);
const Vector4 Debug::WHITE	= Vector4(1, 1, 1, 1);

const Vector4 Debug::YELLOW		= Vector4(1, 1, 0, 1);
const Vector4 Debug::MAGENTA	= Vector4(1, 0, 1, 1);
const Vector4 Debug::CYAN		= Vector4(0, 1, 1, 1);


void Debug::Print(const std::string& text, const Vector2&pos, const Vector4& colour, float size) {
	DebugStringEntry newEntry;

	newEntry.data		= text;
	newEntry.position	= pos;
	newEntry.colour		= colour;
	newEntry.size		= size;

	stringEntries.emplace_back(newEntry);
}

void Debug::DrawLine(const Vector3& startpoint, const Vector3& endpoint, const Vector4& colour, float time) {
	DebugLineEntry newEntry;

	newEntry.start	= startpoint;
	newEntry.end	= endpoint;
	newEntry.colour = colour;
	newEntry.time	= time;

	lineEntries.emplace_back(newEntry);
}

void NCL::Debug::DrawAABB(const Vector3& pos, const Vector3& halfSize, const Vector4& colour, float time)
{
	Vector3 pts[8] = { pos + Vector3(halfSize.x, halfSize.y, halfSize.z),
						pos + Vector3(halfSize.x, -halfSize.y, halfSize.z),
						pos + Vector3(-halfSize.x, halfSize.y, halfSize.z),
						pos + Vector3(-halfSize.x, -halfSize.y, halfSize.z),
						pos + Vector3(halfSize.x, halfSize.y, -halfSize.z),
						pos + Vector3(halfSize.x, -halfSize.y, -halfSize.z),
						pos + Vector3(-halfSize.x, halfSize.y, -halfSize.z),
						pos + Vector3(-halfSize.x, -halfSize.y, -halfSize.z)};
	DrawLine(pts[0], pts[1], colour, time);
	DrawLine(pts[0], pts[2], colour, time);
	DrawLine(pts[1], pts[3], colour, time);
	DrawLine(pts[2], pts[3], colour, time);
	DrawLine(pts[4], pts[5], colour, time);
	DrawLine(pts[4], pts[6], colour, time);
	DrawLine(pts[5], pts[7], colour, time);
	DrawLine(pts[6], pts[7], colour, time);
	DrawLine(pts[0], pts[4], colour, time);
	DrawLine(pts[1], pts[5], colour, time);
	DrawLine(pts[2], pts[6], colour, time);
	DrawLine(pts[3], pts[7], colour, time);
}

void Debug::DrawAxisLines(const Matrix4& modelMatrix, float scaleBoost, float time) {
	Matrix4 local = modelMatrix;
	local.SetPositionVector({0, 0, 0});

	Vector3 fwd		= local * Vector4(0, 0, -1, 1.0f);
	Vector3 up		= local * Vector4(0, 1, 0, 1.0f);
	Vector3 right	= local * Vector4(1, 0, 0, 1.0f);

	Vector3 worldPos = modelMatrix.GetPositionVector();

	DrawLine(worldPos, worldPos + (right * scaleBoost)	, Debug::RED, time);
	DrawLine(worldPos, worldPos + (up * scaleBoost)		, Debug::GREEN, time);
	DrawLine(worldPos, worldPos + (fwd * scaleBoost)	, Debug::BLUE, time);
}


void Debug::FlushRenderables(float dt) {
	if (!renderer) {
		return;
	}
	for (const auto& i : stringEntries) {
		renderer->DrawString(i.data, i.position, i.colour, i.size);
	}
	int trim = 0;
	for (int i = 0; i < lineEntries.size(); ) {
		DebugLineEntry* e = &lineEntries[i]; 
		renderer->DrawLine(e->start, e->end, e->colour);
		e->time -= dt;
		if (e->time < 0) {			
			trim++;				
			lineEntries[i] = lineEntries[lineEntries.size() - trim];
		}
		else {
			++i;
		}		
		if (i + trim >= lineEntries.size()) {
			break;
		}
	}
	lineEntries.resize(lineEntries.size() - trim);

	stringEntries.clear();
}
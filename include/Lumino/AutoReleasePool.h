
#pragma once
#include <vector>
#include <Lumino/Base/RefObject.h>

namespace Lumino
{

/**
	@brief		自動 Release するオブジェクトを管理するプール。
*/
class AutoReleasePool
{
public:

	/**
		@brief		カレントの AutoRseleasePool を取得します。
	*/
	static AutoReleasePool* GetCurrent();

	/**
		@brief		この AutoRseleasePool にオブジェクトを追加します。
	*/
	void AddObject(RefObject* obj);

	/**
		@brief		追加されているオブジェクトの Release() を呼び出し、プールをクリアします。
	*/
	void Clear(RefObject* obj);

private:
	AutoReleasePool();
	~AutoReleasePool();

	std::vector<RefObject*>	m_objectList;
};

} // namespace Lumino

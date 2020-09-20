#include "DLL_MAIN.h"

class IBasePlayer;




class IHandleEntity;



#define NUM_ENT_ENTRY_BITS         (11 + 2)
#define NUM_ENT_ENTRIES            (1 << NUM_ENT_ENTRY_BITS)
#define INVALID_EHANDLE_INDEX       0xFFFFFFFF
#define NUM_SERIAL_NUM_BITS        16 // (32 - NUM_ENT_ENTRY_BITS)
#define NUM_SERIAL_NUM_SHIFT_BITS (32 - NUM_SERIAL_NUM_BITS)
#define ENT_ENTRY_MASK             (( 1 << NUM_SERIAL_NUM_BITS) - 1)

class CCBaseHandle
{
public:
	CCBaseHandle();
	CCBaseHandle(const CCBaseHandle& other);
	CCBaseHandle(unsigned long value);
	CCBaseHandle(int iEntry, int iSerialNumber);

	void Init(int iEntry, int iSerialNumber);
	void Init(int iEntry, int iSerialNumber, int iBits);
	void Term();

	// Even if this returns true, Get() still can return return a non-null value.
	// This just tells if the handle has been initted with any values.
	bool IsValid() const;

	int GetEntryIndex() const;
	int GetSerialNumber() const;

	int ToInt() const;
	bool operator !=(const CCBaseHandle& other) const;
	bool operator ==(const CCBaseHandle& other) const;
	bool operator ==(const IHandleEntity* pEnt) const;
	bool operator !=(const IHandleEntity* pEnt) const;
	bool operator <(const CCBaseHandle& other) const;
	bool operator <(const IHandleEntity* pEnt) const;

	// Assign a value to the handle.
	const CCBaseHandle& operator=(const IHandleEntity* pEntity);
	const CCBaseHandle& Set(const IHandleEntity* pEntity);

	// Use this to dereference the handle.
	// Note: this is implemented in game code (ehandle.h)
	IHandleEntity* Get() const;

protected:
	// The low NUM_SERIAL_BITS hold the index. If this value is less than MAX_EDICTS, then the entity is networkable.
	// The high NUM_SERIAL_NUM_BITS bits are the serial number.
	unsigned long	m_Index;
};










template <typename T>
T zcall_v_func(void* table, int index) {
	return (*(T**)table)[index];
}

class CEntityList
{
public:
	virtual void Function0() = 0;
	virtual void Function1() = 0;
	virtual void Function2() = 0;

	virtual IBasePlayer* GetClientEntity(int iIndex) = 0;
	virtual IBasePlayer* GetClientEntityFromHandle(uint32_t hHandle) = 0;
	virtual int NumberOfEntities(bool bIncludeNonNetworkable) = 0;
	virtual int GetHighestEntityIndex() = 0;


	IBasePlayer* Shonax_GetClientEntityFromHandle(CCBaseHandle hEnt)
	{
		typedef IBasePlayer* (__thiscall* OriginalFn)(PVOID, CCBaseHandle);
		return zcall_v_func<OriginalFn>(this, 4)(this, hEnt);
	}
};

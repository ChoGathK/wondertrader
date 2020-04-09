#pragma once
#include "DataDefine.h"

#include "../Share/IDataWriter.h"
#include "../Share/BoostDefine.h"
#include "../Share/StdUtils.hpp"
#include "../Share/BoostMappingFile.hpp"

#include <queue>
#include <unordered_map>

typedef boost::shared_ptr<BoostMappingFile> BoostMFPtr;

USING_NS_OTP;

class WtDataWriter : public IDataWriter
{
public:
	WtDataWriter();
	~WtDataWriter();	

private:
	template<typename HeaderType, typename T>
	void* resizeRTBlock(BoostMFPtr& mfPtr, uint32_t nCount);

	void  proc_loop();

	void  check_loop();


public:
	virtual bool init(WTSVariant* params, IDataWriterSink* sink) override;
	virtual void release() override;

	virtual bool writeTick(WTSTickData* curTick, bool bNeedProc = true) override;

	virtual bool writeOrderQueue(WTSOrdQueData* curOrdQue) override;

	virtual bool writeOrderDetail(WTSOrdDtlData* curOrdDetail) override;

	virtual bool writeTransaction(WTSTransData* curTrans) override;

	virtual void transHisData(const char* sid) override;
	
	virtual bool isSessionProceeded(const char* sid) override;

	virtual WTSTickData* getCurTick(const char* code) override;

private:
	IDataWriterSink*	_sink;
	IBaseDataMgr*		_bd_mgr;

	typedef struct _KBlockPair
	{
		RTKlineBlock*	_block;
		BoostMFPtr		_file;
		BoostUniqueMutex	_mutex;
		uint64_t		_lasttime;

		_KBlockPair()
		{
			_block = NULL;
			_file = NULL;
			_lasttime = 0;
		}

	} KBlockPair;
	typedef std::unordered_map<std::string, KBlockPair>	KBlockFilesMap;

	typedef struct _TickBlockPair
	{
		RTTickBlock*	_block;
		BoostMFPtr		_file;
		BoostUniqueMutex	_mutex;
		uint64_t		_lasttime;

		boost::shared_ptr< std::ofstream>	_fstream;

		_TickBlockPair()
		{
			_block = NULL;
			_file = NULL;
			_fstream = NULL;
			_lasttime = 0;
		}
	} TickBlockPair;
	typedef std::unordered_map<std::string, TickBlockPair>	TickBlockFilesMap;

	typedef struct _TransBlockPair
	{
		RTTransBlock*	_block;
		BoostMFPtr		_file;
		BoostUniqueMutex	_mutex;
		uint64_t		_lasttime;

		boost::shared_ptr< std::ofstream>	_fstream;

		_TransBlockPair()
		{
			_block = NULL;
			_file = NULL;
			_fstream = NULL;
			_lasttime = 0;
		}
	} TransBlockPair;
	typedef std::unordered_map<std::string, TransBlockPair>	TransBlockFilesMap;

	typedef struct _OdeDtlBlockPair
	{
		RTOrdDtlBlock*	_block;
		BoostMFPtr		_file;
		BoostUniqueMutex	_mutex;
		uint64_t		_lasttime;

		boost::shared_ptr< std::ofstream>	_fstream;

		_OdeDtlBlockPair()
		{
			_block = NULL;
			_file = NULL;
			_fstream = NULL;
			_lasttime = 0;
		}
	} OrdDtlBlockPair;
	typedef std::unordered_map<std::string, OrdDtlBlockPair>	OrdDtlBlockFilesMap;

	typedef struct _OdeQueBlockPair
	{
		RTOrdQueBlock*	_block;
		BoostMFPtr		_file;
		BoostUniqueMutex	_mutex;
		uint64_t		_lasttime;

		boost::shared_ptr< std::ofstream>	_fstream;

		_OdeQueBlockPair()
		{
			_block = NULL;
			_file = NULL;
			_fstream = NULL;
			_lasttime = 0;
		}
	} OrdQueBlockPair;
	typedef std::unordered_map<std::string, OrdQueBlockPair>	OrdQueBlockFilesMap;
	

	KBlockFilesMap	_rt_min1_blocks;
	KBlockFilesMap	_rt_min5_blocks;

	TickBlockFilesMap	_rt_ticks_blocks;
	TransBlockFilesMap	_rt_trans_blocks;
	OrdDtlBlockFilesMap _rt_orddtl_blocks;
	OrdQueBlockFilesMap _rt_ordque_blocks;

	BoostUniqueMutex	_mtx_tick_cache;
	std::unordered_map<std::string, uint32_t> _tick_cache_idx;
	BoostMFPtr		_tick_cache_file;
	RTTickCache*	_tick_cache_block;

	typedef std::function<void()> TaskInfo;
	std::queue<TaskInfo>	_tasks;
	StdThreadPtr			_task_thrd;
	BoostUniqueMutex		_task_mtx;
	BoostCondition			_task_cond;

	std::string		_base_dir;
	std::string		_cache_file;
	uint32_t		_log_group_size;
	bool			_async_proc;

	BoostCondition	 _proc_cond;
	BoostUniqueMutex _proc_mtx;
	std::queue<std::string> _proc_que;
	BoostThreadPtr	_proc_thrd;
	BoostThreadPtr	_proc_chk;
	bool			_terminated;

	bool			_save_tick_log;
	
	std::map<std::string, uint32_t> _proc_date;

private:
	void loadCache();

	bool updateCache(WTSTickData* curTick, bool bNeedProc = true);

	void pipeToTicks(WTSTickData* curTick);

	void pipeToKlines(WTSTickData* curTick);

	KBlockPair* getKlineBlock(const char* code, WTSKlinePeriod period, bool bAutoCreate = true);

	TickBlockPair* getTickBlock(const char* code, uint32_t curDate, bool bAutoCreate = true);
	TransBlockPair* getTransBlock(const char* code, uint32_t curDate, bool bAutoCreate = true);
	OrdDtlBlockPair* getOrdDtlBlock(const char* code, uint32_t curDate, bool bAutoCreate = true);
	OrdQueBlockPair* getOrdQueBlock(const char* code, uint32_t curDate, bool bAutoCreate = true);

	template<typename T>
	void	releaseBlock(T* block);

	void pushTask(TaskInfo task);
};

#pragma once
#include "../WtBtCore/MfMocker.h"

class PyMfMocker : public MfMocker
{
public:
	PyMfMocker(HisDataReplayer* replayer, const char* name);
	virtual ~PyMfMocker();

public:
	virtual void on_init() override;

	virtual void on_session_begin() override;

	virtual void on_session_end() override;

	virtual void on_tick_updated(const char* stdCode, WTSTickData* newTick) override;

	virtual void on_bar_close(const char* stdCode, const char* period, WTSBarStruct* newBar) override;

	virtual void on_strategy_schedule(uint32_t curDate, uint32_t curTime) override;
};


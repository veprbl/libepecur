#ifndef __THINOUTHOOK_HPP
#define __THINOUTHOOK_HPP

#include <epecur/cxx11_compat.hpp>
#include <epecur/geometry.hpp>
#include <epecur/types.hpp>
#include <epecur/StdHits.hpp>

template<typename H>
class ThinOutHook : public H
{
private:

	int     thin_out_factor;
	long	event_id;

	bool	passthrough;

public:

	ThinOutHook(Geometry &g, StdHits::calibration_curve_t *c = NULL, int _thin_out_factor = 1)
	: H(g, c),
	  thin_out_factor(_thin_out_factor),
	  event_id(0),
	  passthrough(true)
	{
		// Nothing
	}

	virtual void	handle_timestamp(int32_t timestamp) override
	{
		if (passthrough) H::handle_timestamp(timestamp);
	}

	virtual void	handle_prop_data(const wire_id_t* begin, const wire_id_t* end, device_id_t dev_id) override
	{
		if (passthrough) H::handle_prop_data(begin, end, dev_id);
	}

	virtual void	handle_drift_data(
		std::vector<wire_id_t> &wire_id,
		std::vector<uint16_t> &time,
		device_id_t dev_id
		) override
	{
		if (passthrough) H::handle_drift_data(wire_id, time, dev_id);
	}

	virtual void	handle_trig_end_cycle() override
	{
		if (passthrough) H::handle_trig_end_cycle();
	}

	virtual void	handle_trig_info(
		uint8_t devices_mask,
		uint16_t event_cause,
		uint32_t gate_time
		) override
	{
		if (passthrough) H::handle_trig_info(devices_mask, event_cause, gate_time);
	}

	virtual void	handle_event_end() override
	{
		if (passthrough) H::handle_event_end();
		event_id++;
		passthrough = ((event_id % thin_out_factor) == 0);
	}
};

#endif

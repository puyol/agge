#pragma once

#include "path.h"
#include "vertex_sequence.h"

namespace agge
{
	typedef pod_vector<point_r> points;

	class stroke : noncopyable
	{
	public:
		struct cap;
		struct join;

	public:
		stroke();
		~stroke();

		void remove_all();
		void add_vertex(real_t x, real_t y, int command);
		
		int vertex(real_t *x, real_t *y);
		
		void width(real_t w);

		template <typename CapT>
		void set_cap(const CapT &c);

		template <typename JoinT>
		void set_join(const JoinT &j);

	private:
		enum state {
			// Stages
			start_cap = 0x00,
			open_outline_forward = 0x01,
			closed_outline_forward = 0x02,
			end_poly1 = 0x03,
			end_cap = 0x04,
			outline_backward = 0x05,
			end_poly = 0x06,
			stop = 0x07,
			stage_mask = 0x07,

			// Flags
			closed = 0x10,
			moveto = 0x20,
			ready = 0x40
		};

	private:
		bool prepare();
		bool is_closed() const;
		void set_state(int stage_and_flags);
		void close();

	private:
		vertex_sequence _input;
		points _output;
		vertex_sequence::const_iterator _i;
		points::const_iterator _o;
		const cap *_cap;
		const join *_join;
		real_t _width;
		int _state;
	};

	struct stroke::cap
	{
		virtual ~cap() { }
		virtual void calc(points &output, real_t w, const point_r &v0, real_t d, const point_r &v1) const = 0;
	};

	struct stroke::join
	{
		virtual ~join() { }
		virtual void calc(points &output, real_t w, const point_r &v0, real_t d01, const point_r &v1, real_t d12, const point_r &v2) const = 0;
	};


	template <typename SourceT, typename GeneratorT>
	class path_generator_adapter : noncopyable
	{
	public:
		path_generator_adapter(SourceT &source, GeneratorT &generator);

		void rewind(int /*path_id*/) { /*not implemented*/ }
		int vertex(real_t *x, real_t *y);

	private:
		enum state { initial = 0, accumulate = 1, generate = 2, stage_mask = 3, complete = 4 };

	private:
		void set_stage(state stage, bool force_complete = false);

	private:
		SourceT &_source;
		GeneratorT &_generator;
		real_t _start_x, _start_y;
		int _state;
	};



	template <typename CapT>
	inline void stroke::set_cap(const CapT &c)
	{
		const CapT *replacement = new CapT(c);
		
		delete _cap;
		_cap = replacement;
	}

	template <typename JoinT>
	inline void stroke::set_join(const JoinT &j)
	{
		const JoinT *replacement = new JoinT(j);
		
		delete _join;
		_join = replacement;
	}


	template <typename SourceT, typename GeneratorT>
	inline path_generator_adapter<SourceT, GeneratorT>::path_generator_adapter(SourceT &source, GeneratorT &generator)
		: _source(source), _generator(generator), _state(initial)
	{	}

	template <typename SourceT, typename GeneratorT>
	inline int path_generator_adapter<SourceT, GeneratorT>::vertex(real_t *x, real_t *y)
	{
		int command;

		for (;;)
			switch (_state & stage_mask)
			{
			case initial:
				command = _source.vertex(&_start_x, &_start_y);
				set_stage(accumulate, path_command_stop == command);

			case accumulate:
				if (_state & complete)
					return path_command_stop;

				_generator.remove_all();
				_generator.add_vertex(_start_x, _start_y, path_command_move_to);

				for (;;)
				{
					real_t xx, yy;

					command = _source.vertex(&xx, &yy);
					if (path_command_move_to == command)
					{
						_start_x = xx;
						_start_y = yy;
					}
					else if (path_command_stop != command)
					{
						_generator.add_vertex(xx, yy, command);
						continue;
					}
					break;
				}
				set_stage(generate, path_command_stop == command);

			case generate:
				command = _generator.vertex(x, y);
				if (path_command_stop != command)
					return command;
				set_stage(accumulate);
			}
	}

	template <typename SourceT, typename GeneratorT>
	inline void path_generator_adapter<SourceT, GeneratorT>::set_stage(state stage, bool force_complete)
	{	_state = (stage & stage_mask) | (force_complete ? complete : (_state & complete));	}
}

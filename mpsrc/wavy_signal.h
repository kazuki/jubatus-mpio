//
// mpio wavy signal
//
// Copyright (C) 2008-2010 FURUHASHI Sadayuki
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//
#ifndef WAVY_SIGNAL_H__
#define WAVY_SIGNAL_H__

#include "wavy_loop.h"
#include <signal.h>

namespace mp {
namespace wavy {
namespace {


struct kernel_signal {
	kernel_signal(kernel& kern, int signo)
	{
		if(kern.add_signal(&m_signal, signo) < 0) {
			throw system_error(errno, "failed to create signal event");
		}
	}

	~kernel_signal() { }

protected:
	int signal_ident() const
	{
		return m_signal.ident();
	}

	int read_signal(event& e)
	{
		return kernel::read_signal( static_cast<event_impl&>(e).get_kernel_event() );
	}

private:
	kernel::signal m_signal;
};


class signal_handler : kernel_signal, public basic_handler {
public:
	typedef loop::signal_callback_t signal_callback_t;

	signal_handler(kernel& kern, int signo,
				signal_callback_t callback) :
		kernel_signal(kern, signo),
		basic_handler(signal_ident(), this),
		m_signo(signo), m_callback(callback)
	{ }

	~signal_handler() { }

	bool operator() (event& e)
	{
		read_signal(e);
		return m_callback(m_signo);
	}

private:
	int m_signo;
	signal_callback_t m_callback;
};


}  // noname namespace
}  // namespace wavy
}  // namespace mp

#endif /* wavy_signal.h */


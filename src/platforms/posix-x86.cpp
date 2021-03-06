#if !defined(_WIN32) && defined(__i386__) && !defined(__x86_64__)

#include <signal.h>

#include "breakpoint.hpp"
#include "posix-common.hpp"
#include "../exmm/platform.hpp"

bool ExMM::Posix::IsMemoryWriteAccess(void* _context)
{
    ucontext_t* context = reinterpret_cast<ucontext_t*>(_context);
    return context->uc_mcontext.gregs[REG_ERR] & 2;
}

void* ExMM::Posix::GetInstructionAddress(void* _context)
{
    ucontext_t* context = reinterpret_cast<ucontext_t*>(_context);
    return reinterpret_cast<void*>(context->uc_mcontext.gregs[REG_EIP]);
}

void ExMM::Platform::InstallBreakPoint(void* context, void* instruction, IoSpace* ioSpace)
{
    BreakPointData<>::Get().Set(ioSpace);
    reinterpret_cast<ucontext_t*>(context)->uc_mcontext.gregs[REG_EFL] |= 0x100;
}

void ExMM::Platform::InstallBreakPoint(void* context, void* instruction, IoSpace* ioSpace, ControllerInterface* controller,
    size_t offset)
{
    BreakPointData<>::Get().Set(ioSpace, controller, offset);
    reinterpret_cast<ucontext_t*>(context)->uc_mcontext.gregs[REG_EFL] |= 0x100;
}

void ExMM::Platform::UninstallBreakPoint(void* context)
{
    BreakPointData<>::Get().Unset();
    reinterpret_cast<ucontext_t*>(context)->uc_mcontext.gregs[REG_EFL] &=~ 0x100;
}

bool ExMM::Platform::GetBreakPoint(void* context, IoSpace*& ioSpace, ControllerInterface*& controller, size_t& offset)
{
    (void)context;
    auto& breakPointData = BreakPointData<>::Get();
    if (breakPointData.Active)
    {
        ioSpace = breakPointData.IoSpace;
        controller = breakPointData.Controller;
        offset = breakPointData.Offset;
        return true;
    }
    return false;
}

#endif

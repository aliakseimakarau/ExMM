#ifndef _EXMM_PLATFORM_H_
#define _EXMM_PLATFORM_H_

namespace ExMM
{
    enum class HookTypes;

    struct IoSpace
    {
        virtual void* GetPublicArea() = 0;
        virtual void* GetPrivateArea() = 0;
        virtual ~IoSpace() = default;
    };

    class Platform
    {
    public:
        static IoSpace* AllocateIoSpace(size_t size, ExMM::HookTypes hookTypes);
        static void RegisterHandlers();
    };
}

#endif // _EXMM_PLATFORM_H_

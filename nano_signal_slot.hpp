#pragma once

#include "nano_function.hpp"
#include "nano_observer.hpp"

namespace Nano
{
    template<typename RT, typename MT_Policy = ST_Policy>
    class Signal;
    template<typename RT, typename MT_Policy, typename... Args>
    class Signal<RT(Args...), MT_Policy> final : public Observer<MT_Policy>
    {
        using SignalType = Signal<RT(Args...), MT_Policy>;

    public:
        class Handler
        {
            friend class SignalType;

        public:
            auto& GetSignal()
            {
                return m_signal;
            }

            template<typename... Uref>
            void Emit(Uref&&... args)
            {
                m_signal.fire(std::forward<Uref>(args)...);
            }

            void ClearSignal()
            {
                m_signal.disconnect_all();
            }

            bool IsEmpty() const
            {
                return m_signal.IsEmpty();
            }

        private:
            SignalType m_signal;
        };

    private:
        using observer = Observer<MT_Policy>;
        using function = Function<RT(Args...)>;

        template<typename T>
        void insert_sfinae(Delegate_Key const& key, typename T::Observer* instance)
        {
            observer::insert(key, instance);
            instance->insert(key, this);
        }
        template<typename T>
        void remove_sfinae(Delegate_Key const& key, typename T::Observer* instance)
        {
            observer::remove(key);
            instance->remove(key);
        }
        template<typename T>
        void insert_sfinae(Delegate_Key const& key, ...)
        {
            observer::insert(key, this);
        }
        template<typename T>
        void remove_sfinae(Delegate_Key const& key, ...)
        {
            observer::remove(key);
        }

    public:
        //-------------------------------------------------------------------CONNECT

        template<typename L>
        void Connect(L* instance)
        {
            observer::insert(function::template bind(instance), this);
        }
        template<typename L>
        void Connect(L& instance)
        {
            Connect(std::addressof(instance));
        }

        template<RT (*fun_ptr)(Args...)>
        void Connect()
        {
            observer::insert(function::template bind<fun_ptr>(), this);
        }

        template<typename T, RT (T::*mem_ptr)(Args...)>
        void Connect(T* instance)
        {
            insert_sfinae<T>(function::template bind<mem_ptr>(instance), instance);
        }
        template<typename T, RT (T::*mem_ptr)(Args...) const>
        void Connect(T* instance)
        {
            insert_sfinae<T>(function::template bind<mem_ptr>(instance), instance);
        }

        template<typename T, RT (T::*mem_ptr)(Args...)>
        void Connect(T& instance)
        {
            Connect<mem_ptr, T>(std::addressof(instance));
        }
        template<typename T, RT (T::*mem_ptr)(Args...) const>
        void Connect(T& instance)
        {
            Connect<mem_ptr, T>(std::addressof(instance));
        }

        template<auto mem_ptr, typename T>
        void Connect(T* instance)
        {
            insert_sfinae<T>(function::template bind<mem_ptr>(instance), instance);
        }
        template<auto mem_ptr, typename T>
        void Connect(T& instance)
        {
            Connect<mem_ptr, T>(std::addressof(instance));
        }

        //----------------------------------------------------------------DISCONNECT

        template<typename L>
        void Disconnect(L* instance)
        {
            observer::remove(function::template bind(instance));
        }
        template<typename L>
        void Disconnect(L& instance)
        {
            Disconnect(std::addressof(instance));
        }

        template<RT (*fun_ptr)(Args...)>
        void Disconnect()
        {
            observer::remove(function::template bind<fun_ptr>());
        }

        template<typename T, RT (T::*mem_ptr)(Args...)>
        void Disconnect(T* instance)
        {
            remove_sfinae<T>(function::template bind<mem_ptr>(instance), instance);
        }
        template<typename T, RT (T::*mem_ptr)(Args...) const>
        void Disconnect(T* instance)
        {
            remove_sfinae<T>(function::template bind<mem_ptr>(instance), instance);
        }

        template<typename T, RT (T::*mem_ptr)(Args...)>
        void Disconnect(T& instance)
        {
            Disconnect<T, mem_ptr>(std::addressof(instance));
        }
        template<typename T, RT (T::*mem_ptr)(Args...) const>
        void Disconnect(T& instance)
        {
            Disconnect<T, mem_ptr>(std::addressof(instance));
        }

        template<auto mem_ptr, typename T>
        void Disconnect(T* instance)
        {
            remove_sfinae<T>(function::template bind<mem_ptr>(instance), instance);
        }
        template<auto mem_ptr, typename T>
        void Disconnect(T& instance)
        {
            Disconnect<mem_ptr, T>(std::addressof(instance));
        }

        //----------------------------------------------------FIRE / FIRE ACCUMULATE

    private:
        template<typename... Uref>
        void fire(Uref&&... args)
        {
            observer::template for_each<function>(std::forward<Uref>(args)...);
        }

        template<typename Accumulate, typename... Uref>
        void fire_accumulate(Accumulate&& accumulate, Uref&&... args)
        {
            observer::template for_each_accumulate<function, Accumulate>(std::forward<Accumulate>(accumulate), std::forward<Uref>(args)...);
        }
    };
} // namespace Nano

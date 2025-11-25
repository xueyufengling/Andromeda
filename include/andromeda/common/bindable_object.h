#ifndef ANDROMEDA_COMMON_OBJECTCOMPONENT
#define ANDROMEDA_COMMON_OBJECTCOMPONENT

namespace andromeda
{
namespace common
{
/**
 * 使用绑定机制工作的对象，例如OpenGL、OpenAL的各种对象
 */
template<typename Id, typename ObjClass>
class bindable_object
{
private:
	Id prev_bind_id = 0; //绑定本组件之前的上一个组件ID

protected:
	Id obj_id = 0; //本组件的ID

public:
	bindable_object() = default;

	bindable_object(Id existed_id) :
			obj_id(existed_id)
	{

	}

	~bindable_object() = default;

	inline void invalidate()
	{
		ObjClass::invalidate(obj_id);
	}

	inline operator Id()
	{
		return obj_id;
	}

	inline Id id()
	{
		return obj_id;
	}

	template<typename ...Args>
	inline void bind_this(Args ... args)
	{
		ObjClass::bind(obj_id, args...);
	}

	/**
	 * 绑定本对象并记录上次绑定的对象
	 */
	template<typename ...Args>
	__attribute__((always_inline)) inline Id begin_bind(Args ... args)
	{
		prev_bind_id = ObjClass::current_bind();
		ObjClass::bind(obj_id, args...);
		return prev_bind_id;
	}

	/**
	 * 本对象解绑并绑定回上次绑定的对象
	 */
	template<typename ...Args>
	__attribute__((always_inline)) inline void end_bind(Args ... args)
	{
		ObjClass::bind(prev_bind_id, args...);
		prev_bind_id = 0;
	}
};

}
}

#endif //ANDROMEDA_COMMON_OBJECTCOMPONENT

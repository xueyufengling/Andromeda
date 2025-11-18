--类实现库
---@meta class
local module_name = ...
local class = {}
_G[module_name] = class

local stdlib = require("stdlib")

access_modifier = { private = 0, protected = 1, public = 2 }

--成员模型
---@class member_model
local member_model = { __metatable = true }
member_model.__signature = nil                           --成员签名，函数或成员变量名称
member_model.__declaring_class = nil                     --此成员所属类
member_model.__access_modifier = access_modifier.private --成员访问修饰符

---@param signature string
---@return member_model
function member_model:set_name(signature)
    self.__signature = signature
    self.__tostring = self.__declaring_class.__tostring .. "::" .. signature
    return self
end

--类模型
---@class class_model
local class_model = { __metatable = true }
class_model.__class_name = nil --类名
class_model.__base_class = {}  --基类数组

---@param class_name string
---@return class_model
function class_model:set_name(class_name)
    self.__class_name = class_name;
    self.__tostring = class_name
    return self
end

---@param access_modifier access_modifier
---@param signature string
---@return member_model
---@nodiscard
function class_model:define_member(access_modifier, signature)
    return stdlib.deepcopy_table(member_model).set_name(signature)
end

--继承的基类列表
function class_model:extends(...)
    stdlib.clear_table(class_model.__base_class)
    for idx, base_class in ipairs({ ... }) do
        stdlib.concat_to_table(base_class, class_model.__base_class)
    end
end

--定义一个类
---@param class_name string
---@return class_model
---@nodiscard
function class:define(class_name)
    local c = stdlib.deepcopy_table(class_model)
    c.__class_name = class_name
    return c
end

--获取元数据（类模型）
function class:metadata(object)
    return getmetatable(object)
end

--对象模型
local object_model = {}

return class

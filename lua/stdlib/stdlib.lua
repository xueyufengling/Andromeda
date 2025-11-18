--标准工具库
---@meta stdlib
local module_name = ...
local stdlib = {}
_G[module_name] = stdlib

--清除table全部数据
function stdlib:clear_table(table)
    local length = #table
    for i = 1, length do
        table[i] = nil
    end
end

--将元素或数组拼接到指定table末尾
---@return table dest
function stdlib:concat_to_table(src, dest)
    local dest_length = #dest
    if type(table) == "table" then
        table.move(src, 1, #src, dest_length + 1, dest)
    else
        dest[dest_length + 1] = src
    end
    return dest
end

--深拷贝table，如果不是table则直接返回此对象
function stdlib:deepcopy_table(table)
    if type(table) == "table" then
        local copy = {}
        for k, v in pairs(table) do
            copy[k] = stdlib.deepcopy_table(v)
        end
        return copy
    else
        return table
    end
end

--浅拷贝table，如果不是table则直接返回此对象
function stdlib:shallowcopy_table(table)
    if type(table) == "table" then
        local copy = {}
        for k, v in pairs(table) do
            copy[k] = v
        end
        return copy
    else
        return table
    end
end

return stdlib

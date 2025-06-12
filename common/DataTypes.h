#include <iostream>
#include <string>
#include <unordered_map>
#include <stack>
#include <vector>

#pragma once

using namespace std;

enum DataType { TYPE_NUMBER, TYPE_STRING, TYPE_ERROR };

string dataTypeToString(DataType dt) {
    switch(dt) {
        case TYPE_NUMBER:    return "number";
        case TYPE_STRING:    return "string";
        case TYPE_ERROR:  return "error";
        default:          return "unknown";
    }
}

DataType combineTypes(DataType left, DataType right) {
    if (left == TYPE_STRING || right == TYPE_STRING) return TYPE_STRING;
    if (left == TYPE_NUMBER   || right == TYPE_NUMBER)   return TYPE_NUMBER;
    if (left == TYPE_ERROR || right == TYPE_ERROR) return TYPE_ERROR;
    return TYPE_ERROR;
}

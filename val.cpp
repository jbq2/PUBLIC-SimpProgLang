#include <string>
#include "val.h"

Value Value::operator+(const Value& op) const {
    Value val;
    if(T == VSTRING && op.T == VSTRING){
        return Value(Stemp + op.Stemp);
    }
    if(T == VINT && (op.T == VINT || op.T == VREAL)){
        if(op.T == VINT){
            return Value(Itemp + op.Itemp);
        }
        else if(op.T == VREAL){
            return Value(Itemp + op.Rtemp);
        }
    }
    else if(T == VREAL && (op.T == VINT || op.T == VREAL)){
        if(op.T == VINT){
            return Value(Rtemp + op.Itemp);
        }
        else if(op.T == VREAL){
            return Value(Rtemp + op.Rtemp);
        }
    }

    return val;
}

Value Value::operator-(const Value& op) const {
    Value val;
    if(T == VINT && (op.T == VINT || op.T == VREAL)){
        if(op.T == VINT){
            return Value(Itemp - op.Itemp);
        }
        else if(op.T == VREAL){
            return Value(Itemp - op.Rtemp);
        }
    }
    else if(T == VREAL && (op.T == VINT || op.T == VREAL)){
        if(op.T == VINT){
            return Value(Rtemp - op.Itemp);
        }
        else if(op.T == VREAL){
            return Value(Rtemp - op.Rtemp);
        }
    }

    return val;
}

Value Value::operator*(const Value& op) const {
    Value val;
    if(T == VINT && (op.T == VINT || op.T == VREAL)){
        if(op.T == VINT){
            return Value(Itemp * op.Itemp);
        }
        else if(op.T == VREAL){
            return Value(Itemp * op.Rtemp);
        }
    }
    else if(T == VREAL && (op.T == VINT || op.T == VREAL)){
        if(op.T == VINT){
            return Value(Rtemp * op.Itemp);
        }
        else if(op.T == VREAL){
            return Value(Rtemp * op.Rtemp);
        }
    }

    return val;
}

Value Value::operator/(const Value& op) const {
    Value val;
    if(T == VINT && (op.T == VINT || op.T == VREAL)){
        if(op.T == VINT){
            return Value(Itemp / op.Itemp);
        }
        else if(op.T == VREAL){
            return Value(Itemp / op.Rtemp);
        }
    }
    else if(T == VREAL && (op.T == VINT || op.T == VREAL)){
        if(op.T == VINT){
            return Value(Rtemp / op.Itemp);
        }
        else if(op.T == VREAL){
            return Value(Rtemp / op.Rtemp);
        }
    }

    return val;
}

Value Value::operator==(const Value& op) const {
    Value val;
    if(T == VSTRING && op.T == VSTRING){
        return Value(Stemp == op.Stemp);
    }
    else if(T == VINT && (op.T == VINT || op.T == VREAL)){
        if(op.T == VINT){
            return Value(Itemp == op.Itemp);
        }
        else if(op.T == VREAL){
            return Value(Itemp == op.Rtemp);
        }
    }
    else if(T == VREAL && (op.T == VINT || op.T == VREAL)){
        if(op.T == VINT){
            return Value(Rtemp == op.Itemp);
        }
        if(op.T == VREAL){
            return Value(Rtemp == op.Rtemp);
        }
    }
    else if(T == VBOOL && op.T == VBOOL){
        return Value(Btemp == op.Btemp);
    }

    return val;
}

Value Value::operator>(const Value& op) const {
    Value val;
    if(T == VINT && (op.T == VINT || op.T == VREAL)){
        if(op.T == VINT){
            return Value(Itemp > op.Itemp);
        }
        else if(op.T == VREAL){
            return Value(Itemp > op.Rtemp);
        }
    }
    else if(T == VREAL && (op.T == VINT || op.T == VREAL)){
        if(op.T == VINT){
            return Value(Rtemp > op.Itemp);
        }
        if(op.T == VREAL){
            return Value(Rtemp > op.Rtemp);
        }
    }

    return val;
}

Value Value::operator<(const Value& op) const {
    Value val;
    if(T == VINT && (op.T == VINT || op.T == VREAL)){
        if(op.T == VINT){
            return Value(Itemp < op.Itemp);
        }
        else if(op.T == VREAL){
            return Value(Itemp < op.Rtemp);
        }
    }
    else if(T == VREAL && (op.T == VINT || op.T == VREAL)){
        if(op.T == VINT){
            return Value(Rtemp < op.Itemp);
        }
        if(op.T == VREAL){
            return Value(Rtemp < op.Rtemp);
        }
    }

    return val;
}


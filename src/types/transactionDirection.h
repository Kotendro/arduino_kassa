#pragma once

enum class TransactionDirection {
    TopToBottom,
    BottomToTop
};

inline TransactionDirection toggleDirection(TransactionDirection dir) {
    return (dir == TransactionDirection::TopToBottom) 
           ? TransactionDirection::BottomToTop 
           : TransactionDirection::TopToBottom;
}
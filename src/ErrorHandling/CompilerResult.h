#pragma once

enum class ResultType{
    Success,
    InvalidToken,
    InvalidSyntax,
    Failure
};

struct CompilerResult {
    CompilerResult(ResultType type){
        Type = type;
    }

    ResultType Type;
};

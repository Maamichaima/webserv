class=$1

headerFile=$1".hpp"
sourceFile=$1".cpp"


cat > $headerFile << EOF
#pragma once

#include <iostream>

class $class
{
    public:
        $class();
        $class(const $class &obj);
        $class &operator=(const $class &obj);
        ~$class();
};
EOF

cat > $sourceFile << EOF
#include "$headerFile"

$class::$class()
{
}

$class::~$class()
{
}

EOF
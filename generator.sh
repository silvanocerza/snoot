#! /bin/bash

# Ugly logs generator, most logs won't make much sense
# Just pipe its output to a file


METHODS=(
    "GET"
    "HEAD"
    "POST"
    "PUT"
    "DELETE"
    "TRACE"
    "OPTIONS"
    "CONNECT"
    "PATCH"
    "BOGUS"
    "WHAT"
    ""
    "AHA"
)
RESOURCES=(
    "/report"
    "/api"
    "/api/user"
    "/docs"
    "/docs/api"
    "/docs/api/tutorial"
    "/docs/api/reference"
    "docs"
    "docs/api"
    "docs/api/reference"
    "2134"
    "docs /api"
)
STATUSES=(
    "200"
    "202"
    "400"
    "404"
    "500"
    "NaN"
    "No"
    "Something"
)

# Returns a random element from the passed argument
# Usage: random_element ARRAY_NAME
random_element() {
    local array_name=$1[@]
    local array=("${!array_name}")
    echo "${array[((RANDOM % ${#array[*]}))]}"
}

while [[ 1 ]]
do
    host="127.0.0.1"
    rfc931="-"
    user=`whoami`
    now=`date +"%d/%b/%Y:%T %z"`
    method=`random_element METHODS`
    resource=`random_element RESOURCES`
    protocol="HTTP/1.0"
    status=`random_element STATUSES`
    size=$(( RANDOM % 200 ))

    echo "$host $rfc931 $user [$now] \"$method $resource $protocol\" $status $size"

    # Sleep for a bit
    sleep $(echo "0.$(( $RANDOM % 50 ))")
done


package main

import (
    "io/ioutil"
)

func main() {
    data, _ := ioutil.ReadFile("source.txt")

    _ = ioutil.WriteFile("go_out.txt", data, 0777)
}

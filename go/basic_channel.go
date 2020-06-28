package main

import (
  "fmt"
  "time")

func main() {

  messages := make(chan string)

  go func() {
    messages <- "ping"
    messages <- "ping"
  }()

  msg := <-messages
  fmt.Println(msg)
}

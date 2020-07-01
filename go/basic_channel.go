// note that pong is not received
package main

import (
  "fmt"
  "time")

func main() {

  messages := make(chan string)

  go func() {
    messages <- "ping"
    messages <- "pong"
  }()

  msg := <-messages
  fmt.Println(msg)
}

// This script shows what happens when a channel is not closed
//
package main

import (
  "fmt"
  "time"
  "sync"
)

func main() {
  const capacity = 5

  messages := make(chan string, capacity)

  var wg sync.WaitGroup
  count := 0

  // spawn five separate threads to shove stuff into the channel
  for i := 0; i<capacity; i++ {
    wg.Add(1)
    go func(i int) {
      fmt.Printf("thread %d: about to ping\n",i)
      messages <- fmt.Sprintf("thread %d: ping",i)
      fmt.Printf("thread %d: ping done\n", i)
      wg.Done()
      count++
      if (count == capacity) {
          fmt.Printf("Sent done, not closing channel\n")
// Uncomment to fix this problem
//        fmt.Printf("Closing channel\n")
//        close(messages)
      }
    }(i)
  }

  fmt.Println("main: about to receive, waiting 2s..")
  time.Sleep(2 * time.Second)

  fmt.Println("main: receiving!")
  for msg := range messages {
    fmt.Println("got: " + msg)
  }
  wg.Wait()
  fmt.Println("main: receive done")
}

package log

import (
	"github.com/stretchr/testify/assert"
	"testing"
)

func TestNewCommandLog(t *testing.T) {
	log := NewCommandLog()
	assert.NotNil(t, log)
}

func TestCommandLogOpen(t *testing.T) {
	log := NewCommandLog()
	err := log.Open("/Users/simon/rewind.log")
	assert.Nil(t, err)
	err = log.Close()
	assert.Nil(t, err)
}

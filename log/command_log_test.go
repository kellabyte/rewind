package log

import (
	"github.com/stretchr/testify/assert"
	"testing"
)

func TestNewCommandLog(t *testing.T) {
	log := NewCommandLog()
	assert.NotNil(t, log)
}

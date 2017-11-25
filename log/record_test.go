package log

import (
	"github.com/stretchr/testify/assert"
	"testing"
)

func TestNewRecord(t *testing.T) {
	entry := NewRecord()
	assert.NotNil(t, entry)
}

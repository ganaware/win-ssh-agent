#!/usr/bin/ruby -Ku

require 'rexml/document'

MAJOR_VERSION = ARGV[0].to_i
MINOR_VERSION = ARGV[1].to_i

TARGET = {
  'agent.manifest' => {
    :xpath => '/assembly/assemblyIdentity/@version',
    :match => "#{MAJOR_VERSION}.#{MINOR_VERSION}.0.0"
  },
  'askpass.manifest' => {
    :xpath => '/assembly/assemblyIdentity/@version',
    :match => "#{MAJOR_VERSION}.#{MINOR_VERSION}.0.0"
  },
  "README.txt" => {
    :line => /^\s+\d\d\d\d\/\d\d\/\d\d\s+(\d+\.\d+)\s*$/,
    :match => sprintf("%d.%02d", MAJOR_VERSION, MINOR_VERSION)
  },
  "README-ja.txt" => {
    :line => /^\s+\d\d\d\d\/\d\d\/\d\d\s+(\d+\.\d+)\s*$/,
    :match => sprintf("%d.%02d", MAJOR_VERSION, MINOR_VERSION)
  },
}

have_error = false

TARGET.each{|filename, desc|
  if desc[:xpath]
    open(filename){|fp|
      doc = REXML::Document.new(fp.read)
      value = REXML::XPath.first(doc, desc[:xpath]).to_s
      if value != desc[:match]
        print "error: #{filename}: #{desc[:xpath]} is #{value} (expected #{desc[:match]})\n"
        have_error = true
      end
    }
  elsif desc[:line]
    open(filename){|fp|
      fp.each_line{|line|
        if line =~ desc[:line]
          value = $1
          if value != desc[:match]
            print "error: #{filename}: #{desc[:line]} is #{value} (expected #{desc[:match]})\n"
            have_error = true
            break
          end
        end
      }
    }
  else
    print "error: #{filename}: "
  end
}

exit 1 if have_error

/*
 * Copyright (c) 2016 - Wizrd Team
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <iostream>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "utils/url.h"
#include <string>


TEST(url_test_case, quote_test)
{
    EXPECT_EQ(Wizrd::URL::quote("http://en.wikipedia.org/wiki/Percent encoding"), "http%3A//en.wikipedia.org/wiki/Percent%20encoding");
    EXPECT_EQ(Wizrd::URL::quote("abc def"), "abc%20def");
}

TEST(url_test_case, quote_plus_test)
{
    EXPECT_EQ(Wizrd::URL::quotePlus("http://en.wikipedia.org/wiki/Percent encoding"), "http%3A//en.wikipedia.org/wiki/Percent+encoding");
    EXPECT_EQ(Wizrd::URL::quotePlus("abc def"), "abc+def");
}

TEST(url_test_case, unquote_test_regular)
{
    EXPECT_EQ(Wizrd::URL::unquote("%20%30"), " 0");
    EXPECT_EQ(Wizrd::URL::unquote("http%3A//en.wikipedia.org/wiki/Percent%20encoding"), "http://en.wikipedia.org/wiki/Percent encoding");
    EXPECT_EQ(Wizrd::URL::unquote("abc%20def"), "abc def");
}

TEST(url_test_case, unquote_test_unregular)
{
    EXPECT_EQ(Wizrd::URL::unquote("%20%30%"), " 0%");
    EXPECT_EQ(Wizrd::URL::unquote("http%3A//%4Hen.wikipedia.org/wiki/Percent%20encoding"), "http://%4Hen.wikipedia.org/wiki/Percent encoding");
    EXPECT_EQ(Wizrd::URL::unquote("%%abc%20def"), "%\xAB""c def");
}

TEST(url_test_case, unquote_plus_test_regular)
{
    EXPECT_EQ(Wizrd::URL::unquotePlus("%20%30"), " 0");
    EXPECT_EQ(Wizrd::URL::unquotePlus("http%3A//en.wikipedia.org/wiki/Percent+encoding"), "http://en.wikipedia.org/wiki/Percent encoding");
    EXPECT_EQ(Wizrd::URL::unquotePlus("abc%20def"), "abc def");
}

TEST(url_test_case, unquote_plus_test_unregular)
{
    EXPECT_EQ(Wizrd::URL::unquotePlus("%20%30%"), " 0%");
    EXPECT_EQ(Wizrd::URL::unquotePlus("http%3A//%4Hen.wikipedia.org/wiki/Percent%20encoding"), "http://%4Hen.wikipedia.org/wiki/Percent encoding");
    EXPECT_EQ(Wizrd::URL::unquotePlus("%%abc+def"), "%\xAB""c def");
}

TEST(url_test_case, url_decode_map_empty)
{
    auto result{Wizrd::URL::decodeMap("")};
    Wizrd::paramsMap expect; // empty map

    EXPECT_EQ(result, expect);
}

TEST(url_test_case, url_decode_map_common)
{
    auto result{Wizrd::URL::decodeMap("foo=bar&ba+=+baz+")};
    Wizrd::paramsMap expect{{"foo", "bar"},
                            {"ba ", " baz "}};
    EXPECT_EQ(result, expect);
}

TEST(url_test_case, url_decode_map_with_some_key_with_no_value)
{
    auto result{Wizrd::URL::decodeMap("foo&ba+=+baz+")};
    Wizrd::paramsMap expect{{"foo", ""},
                            {"ba ", " baz "}};
    EXPECT_EQ(result, expect);
}

TEST(url_test_case, url_decode_empty)
{
    auto result{Wizrd::URL::decode("")};
    Wizrd::params expect;
    EXPECT_EQ(result, expect);
}
TEST(url_test_case, url_decode_item_empty)
{
    auto result{Wizrd::URL::decode("foo=&bar=+")};
    Wizrd::params expect{{"foo", ""}, {"bar", " "}};
    EXPECT_EQ(result, expect);
}


TEST(url_test_case, url_decode_common)
{

    auto result{Wizrd::URL::decode("foo=bar&ba+=+baz+")};
    Wizrd::params expect{{"foo", "bar"}, {"ba ", " baz "}};
    EXPECT_EQ(result, expect);
}

TEST(url_test_case, url_decode_with_some_keys_with_no_value)
{

    auto result{Wizrd::URL::decode("foo&ba+=+baz+")};
    Wizrd::params expect{{"foo"}, {"ba ", " baz "}};
    EXPECT_EQ(result, expect);
}

TEST(url_test_case, url_encode_common)
{
    Wizrd::params params{{"foo", "bar"}, {"  foo  ", "ba@"}};
    Wizrd::paramsMap map{{"foo", "bar"}, {"  foo  ", "ba@"}};
    std::string expect{"foo=bar&++foo++=ba%40"};
    auto result1{Wizrd::URL::encode(params)};
    auto result2{Wizrd::URL::encode(map)};
    EXPECT_EQ(expect, result1);
    // EXPECT_EQ(result2, expect);
    // not testing that because of the order,
    // try reencoding and see if the result is equal
    EXPECT_EQ(map, Wizrd::URL::decodeMap(result2));
}

TEST(url_test_case, url_encode_empty)
{
    Wizrd::params params;
    Wizrd::paramsMap map;
    std::string expect{""};
    auto result1{Wizrd::URL::encode(params)};
    auto result2{Wizrd::URL::encode(map)};
    EXPECT_EQ(result1, expect);
    EXPECT_EQ(result2, expect);
}

TEST(url_test_case, url_encode_invalid_number_of_parameters_should_throw_URLEncodeError)
{
    // creating a parameter vector with items
    // items should have one or two parameters,
    // items with 0 or more than two parameters should throw URLEncodeError
    Wizrd::params params{{"foo", "bar"}, {"  foo  "}, {"a", "b", "c"}};
    Wizrd::params params2{{}, {"  foo  "}, {"a", "b", "c"}};
    EXPECT_THROW(Wizrd::URL::encode(params), Wizrd::URLEncodeError);
    EXPECT_THROW(Wizrd::URL::encode(params2), Wizrd::URLEncodeError);
}

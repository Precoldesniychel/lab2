#include <gtest/gtest.h>
#include "../include/Sequences/ArraySequence.h"
#include "../include/Sequences/ListSequence.h"
#include "../include/Sequences/SequenceFunctions.h"
#include "../include/Specialized/BitSequence.h"
#include "../include/Common/IEnumerator.h"
#include <vector>
#include <cmath>

static void CleanupSeqOfSeqs(Sequence<Sequence<int>*>* seqOfSeqs) {
    if (!seqOfSeqs) return;
    for (int i = 0; i < seqOfSeqs->GetLength(); ++i) delete seqOfSeqs->Get(i);
    delete seqOfSeqs;
}

TEST(SequenceTest, MutableBasicOperations) {
    MutableArraySequence<int>* seq = new MutableArraySequence<int>();
    seq->Append(10); seq->Append(20); seq->Append(30);
    EXPECT_EQ(seq->GetLength(), 3);
    EXPECT_EQ(seq->Get(0), 10);
    EXPECT_EQ(seq->GetFirst(), 10);
    EXPECT_EQ(seq->GetLast(), 30);

    seq->Prepend(5);
    EXPECT_EQ(seq->Get(0), 5);
    EXPECT_EQ(seq->GetLength(), 4);

    seq->InsertAt(15, 2);
    EXPECT_EQ(seq->Get(2), 15);
    EXPECT_EQ(seq->GetLength(), 5);

    auto sub = seq->GetSubsequence(1, 3);
    ASSERT_EQ(sub->GetLength(), 3);
    EXPECT_EQ(sub->Get(0), 10);
    EXPECT_EQ(sub->Get(1), 15);
    EXPECT_EQ(sub->Get(2), 20);
    delete sub;

    MutableArraySequence<int>* other = new MutableArraySequence<int>();
    other->Append(100); other->Append(200);
    auto concat = seq->Concat(other);
    EXPECT_EQ(concat->GetLength(), 7);
    EXPECT_EQ(concat->Get(5), 100);
    delete other;
    delete concat;
}

TEST(SequenceTest, ImmutableBehavior) {
    Sequence<int>* seq = new ImmutableArraySequence<int>();
    seq = seq->Append(1);
    seq = seq->Append(2);
    seq = seq->Append(3);

    Sequence<int>* modified = seq->Append(4);
    EXPECT_EQ(seq->GetLength(), 3);
    EXPECT_EQ(modified->GetLength(), 4);
    EXPECT_EQ(modified->Get(3), 4);

    Sequence<int>* prepended = seq->Prepend(0);
    EXPECT_EQ(prepended->Get(0), 0);
    EXPECT_EQ(seq->Get(0), 1);

    Sequence<int>* inserted = seq->InsertAt(99, 1);
    EXPECT_EQ(inserted->Get(1), 99);
    EXPECT_EQ(seq->Get(1), 2);

    delete seq;
    delete modified;
    delete prepended;
    delete inserted;
}

TEST(SequenceTest, ExceptionHandling) {
    MutableArraySequence<int>* seq = new MutableArraySequence<int>();
    seq->Append(1); seq->Append(2);

    EXPECT_THROW(seq->Get(-1), std::out_of_range);
    EXPECT_THROW(seq->Get(2), std::out_of_range);
    EXPECT_THROW(seq->InsertAt(5, 3), std::out_of_range);
    EXPECT_THROW(seq->GetSubsequence(0, 2), std::out_of_range);
    EXPECT_THROW(seq->GetSubsequence(1, 0), std::out_of_range);

    MutableArraySequence<int>* empty = new MutableArraySequence<int>();
    EXPECT_THROW(empty->GetFirst(), std::out_of_range);
    EXPECT_THROW(empty->GetLast(), std::out_of_range);

    delete seq;
    delete empty;
}

TEST(SequenceTest, CloneAndOperators) {
    MutableListSequence<int>* list = new MutableListSequence<int>();
    list->Append(7); list->Append(8); list->Append(9);

    auto cloned = list->Clone();
    EXPECT_EQ(cloned->GetLength(), 3);
    EXPECT_EQ(cloned->Get(1), 8);
    cloned->Append(10);
    EXPECT_EQ(list->GetLength(), 3);
    EXPECT_EQ(cloned->GetLength(), 4);
    delete cloned;
    delete list;
}

TEST(SequenceFunctionsTest, MapWhereReduce) {
    MutableArraySequence<int>* seq = new MutableArraySequence<int>();
    seq->Append(1); seq->Append(2); seq->Append(3); seq->Append(4); seq->Append(5);

    auto mapped = SequenceFunctions::Map(seq, [](int x) { return x * x; });
    ASSERT_EQ(mapped->GetLength(), 5);
    EXPECT_EQ(mapped->Get(2), 9);
    delete mapped;

    auto filtered = SequenceFunctions::Where(seq, [](int x) { return x % 2 != 0; });
    ASSERT_EQ(filtered->GetLength(), 3);
    EXPECT_EQ(filtered->Get(0), 1);
    EXPECT_EQ(filtered->Get(2), 5);
    delete filtered;

    int product = SequenceFunctions::Reduce(seq, 1, [](int acc, int val) { return acc * val; });
    EXPECT_EQ(product, 120);
    delete seq;
}

TEST(SequenceFunctionsTest, FlatMapSkipSplitSlice) {
    MutableArraySequence<int>* seq = new MutableArraySequence<int>();
    seq->Append(1); seq->Append(2); seq->Append(3);

    auto flat = SequenceFunctions::FlatMap(seq, [](int x) {
        auto inner = new MutableArraySequence<int>();
        inner->Append(x); inner->Append(x * 10);
        return inner;
    });
    ASSERT_EQ(flat->GetLength(), 6);
    EXPECT_EQ(flat->Get(0), 1);
    EXPECT_EQ(flat->Get(1), 10);
    EXPECT_EQ(flat->Get(4), 3);
    EXPECT_EQ(flat->Get(5), 30);
    delete flat;

    auto skipped = SequenceFunctions::Skip(seq, 1);
    ASSERT_EQ(skipped->GetLength(), 2);
    EXPECT_EQ(skipped->Get(0), 2);
    delete skipped;

    auto split = SequenceFunctions::Split(seq, [](int x) { return x == 2; });
    ASSERT_EQ(split->GetLength(), 2);
    EXPECT_EQ(split->Get(0)->Get(0), 1);
    EXPECT_EQ(split->Get(1)->Get(0), 3);
    CleanupSeqOfSeqs(split);

    MutableArraySequence<int>* replacement = new MutableArraySequence<int>();
    replacement->Append(9); replacement->Append(8);
    auto sliced = SequenceFunctions::Slice(seq, 1, 1, replacement);
    ASSERT_EQ(sliced->GetLength(), 4);
    EXPECT_EQ(sliced->Get(0), 1);
    EXPECT_EQ(sliced->Get(1), 9);
    EXPECT_EQ(sliced->Get(2), 8);
    EXPECT_EQ(sliced->Get(3), 3);
    delete sliced;
    delete replacement;
    delete seq;
}

TEST(SequenceFunctionsTest, ZipUnzip) {
    MutableArraySequence<int>* s1 = new MutableArraySequence<int>();
    s1->Append(1); s1->Append(2); s1->Append(3);
    MutableArraySequence<double>* s2 = new MutableArraySequence<double>();
    s2->Append(1.5); s2->Append(2.5); s2->Append(3.5); s2->Append(4.5);

    auto zipped = SequenceFunctions::Zip(s1, s2);
    ASSERT_EQ(zipped->GetLength(), 3);
    EXPECT_EQ(zipped->Get(0).first, 1);
    EXPECT_DOUBLE_EQ(zipped->Get(0).second, 1.5);
    delete zipped;
    delete s1;
    delete s2;

    MutableArraySequence<Pair<int, double>>* pairs = new MutableArraySequence<Pair<int, double>>();
    pairs->Append(Pair<int, double>(10, 1.1));
    pairs->Append(Pair<int, double>(20, 2.2));
    auto unzipped = SequenceFunctions::Unzip(pairs);
    EXPECT_EQ(unzipped.first->Get(0), 10);
    EXPECT_DOUBLE_EQ(unzipped.second->Get(1), 2.2);
    delete unzipped.first;
    delete unzipped.second;
    delete pairs;
}

TEST(SequenceFunctionsTest, MinMaxAvgMedianInversions) {
    MutableArraySequence<int>* seq = new MutableArraySequence<int>();
    seq->Append(5); seq->Append(1); seq->Append(4); seq->Append(2); seq->Append(3);

    auto mma = SequenceFunctions::GetMinMaxAvg(seq);
    EXPECT_EQ(mma.min, 1);
    EXPECT_EQ(mma.max, 5);
    EXPECT_DOUBLE_EQ(mma.avg, 3.0);

    EXPECT_DOUBLE_EQ(SequenceFunctions::GetMedian(seq), 3.0);
    EXPECT_EQ(SequenceFunctions::CountInversions(seq), 6);

    MutableArraySequence<int>* empty = new MutableArraySequence<int>();
    EXPECT_THROW(SequenceFunctions::GetMinMaxAvg(empty), std::invalid_argument);
    EXPECT_THROW(SequenceFunctions::GetMedian(empty), std::invalid_argument);

    delete seq;
    delete empty;
}

TEST(SequenceFunctionsTest, PrefixesSuffixesMovingAverageMirror) {
    MutableArraySequence<int>* seq = new MutableArraySequence<int>();
    seq->Append(1); seq->Append(2); seq->Append(3);

    auto prefs = SequenceFunctions::GetPrefixes(seq);
    EXPECT_EQ(prefs->GetLength(), 3);
    EXPECT_EQ(prefs->Get(0)->GetLength(), 1);
    EXPECT_EQ(prefs->Get(2)->GetLength(), 3);
    CleanupSeqOfSeqs(prefs);

    auto suffs = SequenceFunctions::GetSuffixes(seq);
    EXPECT_EQ(suffs->GetLength(), 3);
    EXPECT_EQ(suffs->Get(0)->GetLength(), 3);
    EXPECT_EQ(suffs->Get(2)->GetLength(), 1);
    CleanupSeqOfSeqs(suffs);

    auto ma = SequenceFunctions::GetMovingAverage(seq, 2);
    ASSERT_EQ(ma->GetLength(), 2);
    EXPECT_EQ(ma->Get(0), 1);
    EXPECT_EQ(ma->Get(1), 2);
    delete ma;

    auto mirror = SequenceFunctions::GetMirrorSum(seq);
    ASSERT_EQ(mirror->GetLength(), 3);
    EXPECT_EQ(mirror->Get(0), 4);
    EXPECT_EQ(mirror->Get(1), 4);
    EXPECT_EQ(mirror->Get(2), 4);
    delete mirror;

    EXPECT_THROW(SequenceFunctions::GetMovingAverage(seq, 0), std::invalid_argument);
    delete seq;
}

TEST(IteratorTest, FullTraversalAndReset) {
    MutableListSequence<int>* list = new MutableListSequence<int>();
    list->Append(10); list->Append(20); list->Append(30);

    auto it = list->GetEnumerator();
    std::vector<int> collected;
    while (it->MoveNext()) collected.push_back(it->GetCurrent());
    ASSERT_EQ(collected.size(), 3);
    EXPECT_EQ(collected[0], 10);
    EXPECT_EQ(collected[2], 30);

    it->Reset();
    EXPECT_TRUE(it->MoveNext());
    EXPECT_EQ(it->GetCurrent(), 10);
    delete it;
    delete list;
}

TEST(IteratorTest, EdgeCases) {
    MutableArraySequence<int>* empty = new MutableArraySequence<int>();
    auto it = empty->GetEnumerator();
    EXPECT_FALSE(it->MoveNext());
    EXPECT_THROW(it->GetCurrent(), std::out_of_range);
    delete it;
    delete empty;

    MutableArraySequence<int>* single = new MutableArraySequence<int>();
    single->Append(42);
    auto it2 = single->GetEnumerator();
    EXPECT_TRUE(it2->MoveNext());
    EXPECT_EQ(it2->GetCurrent(), 42);
    EXPECT_FALSE(it2->MoveNext());
    delete it2;
    delete single;
}

TEST(BitSequenceTest, LogicalOperators) {
    BitSequence* a = new BitSequence();
    a->Append(true); a->Append(false); a->Append(true);
    BitSequence* b = new BitSequence();
    b->Append(true); b->Append(true); b->Append(false);

    auto andRes = *a & *b;
    EXPECT_EQ(andRes->Get(0).value, true);
    EXPECT_EQ(andRes->Get(1).value, false);
    EXPECT_EQ(andRes->Get(2).value, false);
    delete andRes;

    auto orRes = *a | *b;
    EXPECT_EQ(orRes->Get(1).value, true);
    EXPECT_EQ(orRes->Get(2).value, true);
    delete orRes;

    auto xorRes = *a ^ *b;
    EXPECT_EQ(xorRes->Get(0).value, false);
    EXPECT_EQ(xorRes->Get(1).value, true);
    EXPECT_EQ(xorRes->Get(2).value, true);
    delete xorRes;

    auto notRes = !(*a);
    EXPECT_EQ(notRes->Get(0).value, false);
    EXPECT_EQ(notRes->Get(1).value, true);
    delete notRes;
    delete a;
    delete b;
}

TEST(BitSequenceTest, MaskingAndLengthMismatch) {
    BitSequence* mask = new BitSequence();
    mask->Append(true); mask->Append(false); mask->Append(true); mask->Append(true);
    BitSequence* data = new BitSequence();
    data->Append(false); data->Append(true);

    auto masked = *data & *mask;
    EXPECT_EQ(masked->GetLength(), 2);
    EXPECT_EQ(masked->Get(0).value, false);
    EXPECT_EQ(masked->Get(1).value, false);
    delete masked;
    delete mask;
    delete data;

    BitSequence* emptyMask = new BitSequence();
    BitSequence* emptyData = new BitSequence();
    auto emptyAnd = *emptyData & *emptyMask;
    EXPECT_EQ(emptyAnd->GetLength(), 0);
    delete emptyAnd;
    delete emptyMask;
    delete emptyData;
}

// Дополнительные тесты для Try‑семантики
TEST(SequenceFunctionsTest, TrySemantics) {
    MutableArraySequence<int>* seq = new MutableArraySequence<int>();
    seq->Append(1); seq->Append(2); seq->Append(3);

    auto optFirst = SequenceFunctions::TryFirst(seq);
    EXPECT_TRUE(optFirst.IsSome());
    EXPECT_EQ(optFirst.GetValue(), 1);

    auto optFind = SequenceFunctions::TryFind(seq, [](int x) { return x > 2; });
    EXPECT_TRUE(optFind.IsSome());
    EXPECT_EQ(optFind.GetValue(), 3);

    auto optMissing = SequenceFunctions::TryFind(seq, [](int x) { return x > 10; });
    EXPECT_FALSE(optMissing.IsSome());

    MutableArraySequence<int>* empty = new MutableArraySequence<int>();
    auto optEmpty = SequenceFunctions::TryFirst(empty);
    EXPECT_FALSE(optEmpty.IsSome());

    delete seq;
    delete empty;
}